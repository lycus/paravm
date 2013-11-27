#include <linux/cred.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_DESCRIPTION("Kernel support module for ParaVM");
MODULE_VERSION("0.0.0");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("The Lycus Foundation");

#define FILE_NAME "paravm"

struct paravm_entry
{
    void *ptr;
    size_t size;
    struct list_head list;
};

struct paravm_state
{
    struct mutex mutex;
    struct list_head entries;
};

static void paravm_free(struct paravm_entry *entry)
{
    printk(KERN_DEBUG "ParaVM: Freeing physical address '0x%p' with '%s'\n", entry->ptr,
           entry->size >= PAGE_SIZE ? "kfree" : "free_pages");

    if (entry->size >= PAGE_SIZE)
        kfree(entry->ptr);
    else
        free_pages((unsigned long)entry->ptr, get_order(entry->size));
}

static int paravm_open(struct inode *inode, struct file *filp)
{
    struct paravm_state *state;

    if (!capable(CAP_SYS_ADMIN))
        return -EACCES;

    state = kmalloc(sizeof(struct paravm_state), GFP_KERNEL);

    if (!state)
        return -ENOMEM;

    mutex_init(&state->mutex);
    INIT_LIST_HEAD(&state->entries);

    filp->private_data = state;

    printk(KERN_DEBUG "ParaVM: Process '%i' opened '" FILE_NAME "'\n", current->pid);

    return 0;
}

static int paravm_release(struct inode *inode, struct file *filp)
{
    struct paravm_state *state = filp->private_data;
    struct paravm_entry *entry;
    struct paravm_entry *tmp;

    list_for_each_entry_safe(entry, tmp, &state->entries, list)
    {
        paravm_free(entry);

        list_del(&entry->list);
        kfree(entry);
    }

    mutex_destroy(&state->mutex);

    kfree(state);

    printk(KERN_DEBUG "ParaVM: Process '%i' closed '" FILE_NAME "'\n", current->pid);

    return 0;
}

static ssize_t paravm_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
    struct paravm_state *state = filp->private_data;
    int ret = count;
    struct paravm_entry *entry;
    size_t size = 0;

    mutex_lock(&state->mutex);

    list_for_each_entry(entry, &state->entries, list)
        size++;

    size *= (sizeof(void *) + sizeof(size_t));
    size += sizeof(size_t);

    printk(KERN_DEBUG "ParaVM: Memory map size during read is '%zu'\n", size);

    if (count != size)
    {
        ret = -EINVAL;
        goto abort;
    }

    copy_to_user(buf, &size, sizeof(size_t));
    buf += sizeof(size_t);

    list_for_each_entry(entry, &state->entries, list)
    {
        printk(KERN_DEBUG "ParaVM: Sending physical address '0x%p' of size '%zu'\n", entry->ptr, entry->size);

        copy_to_user(buf, &entry->ptr, sizeof(void *));
        buf += sizeof(void *);

        copy_to_user(buf, &entry->size, sizeof(size_t));
        buf += sizeof(size_t);
    }

abort:
    mutex_unlock(&state->mutex);

    return ret;
}

#define USER_OP_ALLOC 0x00
#define USER_OP_FREE 0x01

static ssize_t paravm_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
    struct paravm_state *state = filp->private_data;
    int ret = count;
    u8 op;

    mutex_lock(&state->mutex);

    if (count < sizeof(u8))
    {
        ret = -ENODATA;
        goto abort;
    }

    if (copy_from_user(&op, buf, sizeof(u8)))
    {
        ret = -EFAULT;
        goto abort;
    }

    buf += sizeof(u8);

    printk(KERN_DEBUG "ParaVM: Got request '0x%x' with count '%zu'\n", op, count);

    switch (op)
    {
        case USER_OP_ALLOC:
        {
            size_t size;
            struct paravm_entry *entry;
            void *ptr;

            if (count - sizeof(u8) < sizeof(size_t))
            {
                ret = -ENODATA;
                goto abort;
            }

            if (copy_from_user(&size, buf, sizeof(size_t)))
            {
                ret = -EFAULT;
                goto abort;
            }

            printk(KERN_DEBUG "ParaVM: Process '%i' requested '%zu' bytes\n", current->pid, size);

            if (!size)
            {
                ret = -EINVAL;
                goto abort;
            }

            entry = kmalloc(sizeof(struct paravm_entry), GFP_KERNEL);

            if (!entry)
            {
                printk(KERN_DEBUG "ParaVM: Could not allocate memory map entry\n");

                ret = -ENOMEM;
                goto abort;
            }

            if (size >= PAGE_SIZE)
                ptr = (void *)__get_free_pages(GFP_KERNEL, get_order(size));
            else
                ptr = kmalloc(size, GFP_KERNEL);

            if (!ptr)
            {
                kfree(entry);

                ret = -ENOMEM;
                goto abort;
            }

            entry->ptr = ptr;
            entry->size = size;

            list_add_tail(&entry->list, &state->entries);

            printk(KERN_DEBUG "ParaVM: Allocated physical address '0x%p' of size '%zu' with '%s'\n",
                   ptr, size, size >= PAGE_SIZE ? "kmalloc" : "__get_free_pages");

            break;
        }
        case USER_OP_FREE:
        {
            void *ptr;
            struct paravm_entry *entry;
            struct paravm_entry *tmp;

            if (count - sizeof(u8) < sizeof(void *))
            {
                ret = -ENODATA;
                goto abort;
            }

            if (copy_from_user(&ptr, buf, sizeof(void *)))
            {
                ret = -EFAULT;
                goto abort;
            }

            list_for_each_entry_safe(entry, tmp, &state->entries, list)
            {
                if (entry->ptr == ptr)
                {
                    paravm_free(entry);

                    list_del(&entry->list);
                    kfree(entry);

                    break;
                }
            }

            break;
        }
        default:
            ret = -EBADRQC;
            goto abort;
    }

abort:
    mutex_unlock(&state->mutex);

    return ret;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = paravm_open,
    .release = paravm_release,
    .read = paravm_read,
    .write = paravm_write,
};

static int paravm_major;
static struct class *paravm_class;
static struct device *paravm_device;

static void paravm_cleanup(void)
{
    if (paravm_device)
    {
        device_destroy(paravm_class, MKDEV(paravm_major, 0));

        printk(KERN_DEBUG "ParaVM: Destroyed device '" FILE_NAME "'\n");
    }

    if (paravm_class)
    {
        class_unregister(paravm_class);
        class_destroy(paravm_class);

        printk(KERN_DEBUG "ParaVM: Destroyed device class '" FILE_NAME "'\n");
    }

    if (paravm_major)
    {
        unregister_chrdev(paravm_major, FILE_NAME);

        printk(KERN_DEBUG "ParaVM: Unregistered character device '" FILE_NAME "' with major '%i'\n", paravm_major);
    }
}

static int __init paravm_init(void)
{
    if ((paravm_major = register_chrdev(0, FILE_NAME, &fops)) < 0)
    {
        printk(KERN_ERR "ParaVM: Failed to create character device '" FILE_NAME "'\n");
        goto abort;
    }

    printk(KERN_DEBUG "ParaVM: Registered character device '" FILE_NAME "' with major '%i'\n", paravm_major);

    if (!(paravm_class = class_create(THIS_MODULE, FILE_NAME)))
    {
        printk(KERN_ERR "ParaVM: Failed to create device class '" FILE_NAME "'\n");
        goto abort;
    }

    printk(KERN_DEBUG "ParaVM: Created device class '" FILE_NAME "'\n");

    if (!(paravm_device = device_create(paravm_class, NULL, MKDEV(paravm_major, 0), NULL, FILE_NAME)))
    {
        printk(KERN_ERR "ParaVM: Failed to create device '" FILE_NAME "'\n");
        goto abort;
    }

    printk(KERN_DEBUG "ParaVM: Created device '" FILE_NAME "'\n");

    printk(KERN_INFO "ParaVM: All initialization successful");
    return 0;

abort:
    paravm_cleanup();
    return -1;
}

static void __exit paravm_exit(void)
{
    paravm_cleanup();
}

module_init(paravm_init)
module_exit(paravm_exit)
