#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


#include <linux/kfifo.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/string.h>

#include <linux/uaccess.h>


#include<asm/uaccess.h>


#include<linux/init.h>


#include<linux/moduleparam.h>
#include<linux/fs.h>
#include<linux/buffer_head.h>
#include<asm/segment.h>

#include<linux/slab.h>
#include<linux/kallsyms.h>
#include<linux/stat.h>
#include<linux/proc_fs.h>
#include<linux/seq_file.h>
#include<linux/kdev_t.h>
#include<linux/types.h>




MODULE_LICENSE("GPL");


/* BUFFER RESERVATION */

//cipher buffer
char original_message[4096]="Default Buffer";

//cipher key buffer
char original_buffer [128] = "OrigianlKey";

//cipher key proc buffer
char trial_buffer [128]= "TrialKey";

//cipher encrypted buffer
char message_after_encryption [4096];


/* START PROTOYPES FUNCTIONS */
void rc4(unsigned char * p, unsigned char * k, unsigned char * c,int l);

static int proc_cipher_output(struct seq_file *m, void *v);
 
static int proc_cipher_open(struct inode *inode, struct  file *file); 

static int proc_cipher_key_output(struct seq_file *m, void *v);

static ssize_t proc_cipher_key_write(struct file * f, const char* ch, size_t sa, loff_t* offset);
 
static int proc_cipher_key_open(struct inode *inode, struct  file *file); 

static int device_cipher_output(struct seq_file *m, void *v);

static ssize_t device_cipher_write(struct file * f, const char* ch, size_t sa, loff_t* offset);

static int device_cipher_open(struct inode *inode, struct  file *file);


static int device_cipher_key_output(struct seq_file *m, void *v);

static ssize_t device_cipher_key_write(struct file * f, const char* ch, size_t sa, loff_t* offset);

static int device_cipher_key_open(struct inode *inode, struct  file *file);



/* END PROTOYPES FUNCTIONS */



//rc4 from Dr. karim 
/* START RC4 */
void rc4(unsigned char * p, unsigned char * k, unsigned char * c,int l)
{
        unsigned char s [256];
        unsigned char t [256];
        unsigned char temp;
        unsigned char kk;
        int i,j,x;
        for ( i  = 0 ; i  < 256 ; i ++ )
        {
                s[i] = i;
                t[i]= k[i % 4];
        }
        j = 0 ;
        for ( i  = 0 ; i  < 256 ; i ++ )
        {
                j = (j+s[i]+t[i])%256;
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
        }

        i = j = -1;
        for ( x = 0 ; x < l ; x++ )
        {
                i = (i+1) % 256;
                j = (j+s[i]) % 256;
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
                kk = (s[i]+s[j]) % 256;
                c[x] = p[x] ^ s[kk];
        }
}

/* END RC4 */

//output of cipher proc
static int proc_cipher_output(struct seq_file *myfile, void *v) {
  seq_printf(myfile, "%s\n", original_message);
  return 0;
}

//open proc cipher
static int proc_cipher_open(struct inode *inode, struct  file *file) {
  return single_open(file, proc_cipher_output, NULL);
}

//struct fops of cipher
static const struct file_operations proc_cipher_fops = {
  .owner = THIS_MODULE,
  .open = proc_cipher_open,
  .read = seq_read,
 /* .write=cipherprocwrite this is disabled for cipher in ordere not to write in it */
  .llseek = seq_lseek,
  .release = single_release,
};


//cipher key output
static int proc_cipher_key_output(struct seq_file *m, void *v) {
  seq_printf(m, "Go away silly one, you cannot see my key\n");
  return 0;
}

//cipher key write
static ssize_t proc_cipher_key_write(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(trial_buffer,ch,sa);
  rc4(message_after_encryption,trial_buffer, original_message,4096);
return sa; 
}


//cipher key open
static int proc_cipher_key_open(struct inode *inode, struct  file *file) {
  return single_open(file, proc_cipher_key_output, NULL);
}

//cipher key fops
static const struct file_operations proc_cipher_key_fops = {
  .owner = THIS_MODULE,
  .open = proc_cipher_key_open,
  .read = seq_read,
  .write=proc_cipher_key_write,
  .llseek = seq_lseek,
  .release = single_release,
};

//cipher device output
static int device_cipher_output(struct seq_file *m, void *v) {
  
  seq_printf(m, "%s\n", message_after_encryption);
  return 0;
}


//cipher device write
static ssize_t device_cipher_write(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(original_message,ch,sa);

rc4(original_message,original_buffer, message_after_encryption,4096);
return sa; 
}

//cipher device open
static int device_cipher_open(struct inode *inode, struct  file *file) {
  return single_open(file, device_cipher_output, NULL);
}

//cipher device fops
static const struct file_operations device_cipher_fops = {
  .owner = THIS_MODULE,
  .open = device_cipher_open,
  .read = seq_read,
  .write=device_cipher_write,
  .llseek = seq_lseek,
  .release = single_release,
};


//cipher key device output
static int device_cipher_key_output(struct seq_file *m, void *v) {
  seq_printf(m, "Top Secret Key\n");
  return 0;
}

//cipher key device write
static ssize_t device_cipher_key_write(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(original_buffer,ch,sa);
return sa; 
}

//cipher key device open
static int device_cipher_key_open(struct inode *inode, struct  file *file) {
  return single_open(file, device_cipher_key_output, NULL);
}

//cipher key fops
static const struct file_operations device_cipher_key_fops = {
  .owner = THIS_MODULE,
  .open = device_cipher_key_open,
  .read = seq_read,
  .write=device_cipher_key_write,
  .llseek = seq_lseek,
  .release = single_release,
};

//flags to differenitate between which device is invooked
static int devflag1;
static int devflag2;

//instantiate an array of two elements of type cdev [two devices]
struct cdev chardev[2];



int init_module(void){
		
	proc_create("mycipher", 0644, NULL, &proc_cipher_fops);
	proc_create("mycipher_key", 0644, NULL, &proc_cipher_key_fops);



	
	devflag1 = register_chrdev_region(MKDEV(305,0),2, "mycipher");
	if (devflag1 >= 0)
		printk(KERN_ALERT "cipher: Device Registered\n");
	else 
		printk(KERN_ALERT "cipher: Device Registration failed\n");
	

	cdev_init(&chardev[0], &device_cipher_fops);
	cdev_add (&chardev[0], MKDEV (305,0),1);

	cdev_init(&chardev[1], &device_cipher_key_fops);
	cdev_add (&chardev[1], MKDEV(305,1),1);
	return 0; 
}



void cleanup_module(void){

	remove_proc_entry("cipher", NULL);
	remove_proc_entry("cipher_key", NULL);
	cdev_del(&chardev[0]);
	cdev_del(&chardev[1]);

	unregister_chrdev_region(305, 0);
	unregister_chrdev_region(305, 1);


	
}



