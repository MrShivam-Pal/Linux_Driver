#include <linux/module.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define POP_DATA _IOR('a', 'c', struct data* )
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int* )
#define PUSH_DATA _IOW('a', 'b', struct data* )

static int count = 0;
static int q_size = 0;
static int front = 0;
static int rear = 0;
static char **queue;
static int major_numbr;
static struct class *vicharak_class = NULL;
static struct device *vicharak_device = NULL;



struct data {
	int length;
	char *data;
};

DECLARE_WAIT_QUEUE_HEAD(queue_wait);

static int dev_open(struct inode *inodep , struct file *filep){
	printk(KERN_INFO"Vicharak device is opened\n....");
	return 0;
}

static int dev_release(struct inode *inodep , struct file *filep){
	printk(KERN_INFO"Vicharak device is closed\n....");
	return 0;
}

static long dev_ioctl(struct file *file , unsigned int func , unsigned long datap){
	
	struct data u_data;
	int size;
	
	
	switch(func){
		case PUSH_DATA:
		
			if(count == q_size){
				printk(KERN_ALERT"Queue size is full\n");
				return -ENOMEM;
			}
			
			if(copy_from_user(&u_data , (struct data*)datap , sizeof(struct data))){
				return -EFAULT;
			}
			
			queue[rear] = kmalloc(u_data.length, GFP_KERNEL);
			
			if(copy_from_user(queue[rear] , u_data.data , u_data.length)){
				kfree(queue[rear]);
				return -EFAULT;
			}
			
			rear = (rear+1)%q_size;
			count++;
			wake_up_interruptible(&queue_wait);
			printk(KERN_INFO"Data is pushed in Queue\n");
			break;	
			
		case SET_SIZE_OF_QUEUE:
		
			if(copy_from_user(&size , (int *)datap , sizeof(int))){
				return -EFAULT;
			}
			q_size = size;
			front = rear = count = 0;
			queue = kmalloc(q_size * sizeof(char*) , GFP_KERNEL);
			
			printk(KERN_INFO"Queue size is set: %d\n", q_size);
			break;
			
		case POP_DATA:
			
			if(count == 0){
				wait_event_interruptible(queue_wait, count > 0);
			}
			
			if(copy_from_user(&u_data , (struct data*)datap , sizeof(struct data))){
				return -EFAULT;
			}
			
			if(copy_to_user(u_data.data , queue[front] , strlen(queue[front])+1)){
				return -EFAULT;
			}
			
			kfree(queue[front]);
			
			front = (front+1)%q_size;
			count--;
			
			printk(KERN_INFO"Data is popped from queue\n");
			break;
			
		default:
			return -EINVAL;			
	}
	
	return 0;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.unlocked_ioctl = dev_ioctl,
};


static void __exit vicharak_exit(void){
	device_destroy(vicharak_class , MKDEV(major_numbr , 0));
	class_unregister(vicharak_class);
	class_destroy(vicharak_class);
	unregister_chrdev(major_numbr , "vicharak");
	
	for(int i=0;i<count;i++){
		kfree(queue[(front+i)%q_size]);
	}
	
	kfree(queue);
	
	printk(KERN_INFO"Device is unregister....\n");
}

static int __init vicharak_init(void){
	major_numbr = register_chrdev(0 , "vicharak", &fops);
	
	vicharak_class = class_create("vicharak_class");
	
	vicharak_device = device_create(vicharak_class, NULL , MKDEV(major_numbr ,0) , NULL , "vicharak");
	
	printk(KERN_INFO"Vicharak device is created...\n"); 
	return 0;
}

module_init(vicharak_init);
module_exit(vicharak_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shivam Pal");
