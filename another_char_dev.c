#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>








MODULE_LICENSE("GPL");

static int offensive_out (struct seq_file *m, void *v);


static int my_open(struct inode *, struct file *);
static int my_close(struct inode *, struct file *);
static ssize_t my_read(struct file *, char *, size_t, loff_t *);
static ssize_t my_write(struct file *,  const char *, size_t, loff_t *);


static ssize_t my_read_2(struct file *, char *, size_t, loff_t *);
static ssize_t my_write_2(struct file *,  const char *, size_t, loff_t *);


static int fork_open (struct inode *inode, struct file *f);
static int fork_show(struct seq_file *f, void * p);
static ssize_t zero_the_counter_bonus(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos);


static int proc2_open (struct inode *inode, struct file *f);
static int proc2_show(struct seq_file *f, void * p);
static ssize_t proc2_write(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos);

unsigned char databuf[4096];
 unsigned char  out[4096];
 unsigned char keyy[4096];
 unsigned char proc_in[4096];
 unsigned char* offensive= "get away from here naw mafesh password";

static int write_call = 0;
static int num_writes = 0;
static int file_opens = 0;
static int major_number;

char encrypted_data[256];
static int encrypt_index = 0;
static struct file_operations cipher_fops = 
{
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_close,
};

static struct file_operations key_cipher_fops = 
{
	.read = my_read_2,
	.write = my_write_2,
	.open = my_open,
	.release = my_close,
};

static const struct file_operations proc_cipher_fops ={

.owner= THIS_MODULE,
.open = fork_open,
.read = seq_read,
.llseek= seq_lseek,
.release= single_release,
.write= zero_the_counter_bonus,
};

static const struct file_operations proc_cipher_fops_original ={

.owner= THIS_MODULE,
.open = proc2_open,
.read = seq_read,
.llseek= seq_lseek,
.release= single_release,
.write= proc2_write,
};


struct my_device_data {
    struct cdev cdev;
    /* my data starts here */
    //...

};

struct my_device_data devs[2];
unsigned char * key= "DavyJones";


static ssize_t zero_the_counter_bonus(struct file* file,const char __user *buf,size_t count,loff_t *f_pos){
	
size_t maxdatalen = 4096, ncopied;
    

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(proc_in, buf, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    proc_in[maxdatalen] = 0;

    printk("Data from the user: %s\n", proc_in);

    return count;	
}

static int fork_show(struct seq_file *file_p, void * p){

seq_printf(file_p, " %s \n", proc_in);
return 0;
}

static int fork_open (struct inode *inode, struct file *f){
return single_open(f, offensive_out, NULL);

}



static ssize_t proc2_write(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){
	
	//fork_count=0;
	
}



static int proc2_open (struct inode *inode, struct file *f){
return single_open(f, proc2_show, NULL);

}


static int my_open(struct inode *ip, struct file *fp)
{
	file_opens++;
	printk(KERN_INFO "encdev: device was opened\n");
	return 0;
}


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


static int __init encdev_init(void)
{
	/*major_number = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &file_ops);
	if (major_number >= 0) printk(KERN_INFO "encdev: Device Registered\n");
	else printk(KERN_INFO "encdev: Device Registration failed\n");
	return 0;*/
int major;
major= register_chrdev_region(MKDEV(255,0),2,"cipher");

int major_2;
major_2= register_chrdev_region(MKDEV(255,1),2,"cipher_key");
/*if (major==0){

printk(KERN_INFO "fail \n");
return 0;

}*/


cdev_init(&devs[0].cdev, &cipher_fops);
cdev_add(&devs[0].cdev, MKDEV(255,0),1);

cdev_init(&devs[1].cdev, &key_cipher_fops);
cdev_add(&devs[1].cdev, MKDEV(255,1),1);

//cdev_init(&dev[0], &cipher_fops);
//cdev_add(&dev[0], MKDEV(255,1),1);

proc_create("cipher_key",0, NULL,&proc_cipher_fops);
proc_create("cipher",0, NULL,&proc_cipher_fops_original);
return 0;

}

static void __exit encdev_exit(void)
{
	
	printk(KERN_INFO "encdev: Unregistered the device\n");
remove_proc_entry("cipher", NULL);
remove_proc_entry("cipher_key", NULL);

	cdev_del(&devs[0].cdev);
	unregister_chrdev(MKDEV(255,0), 2);

	cdev_del(&devs[1].cdev);
	unregister_chrdev(MKDEV(255,1), 2);
    //cdev_del(&dev[1]);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	
	size_t maxdatalen = 4096, ncopied;
   

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }



    ncopied = copy_from_user(databuf, buf, maxdatalen);
   rc4(databuf,key, out, 100);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

databuf[maxdatalen]= 0;
  out[maxdatalen] = 0;

    printk("Data from the user: %s\n", databuf);
    printk("Data from the user: %s\n", out);

    return count;
}


static ssize_t my_write_2(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	size_t maxdatalen = 4096, ncopied;
  

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(keyy, buf, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    keyy[maxdatalen] = 0;

    printk("Data from the user: %s\n", keyy);

    return count;
	
}

static int my_close(struct inode *ip, struct file *fp)
{
	/*printk(KERN_INFO "encdev: device was closed\n");
	write_call = 0;
	num_writes = 0;
	encrypt_index = 0;
	return 0;*/
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{



count=4096;

    if (copy_to_user(buf, databuf, count)) {
        return -EFAULT;
    }

  

	 //printk(simple_read_from_buffer(buf, count, offset, out, 4096))


  //  rc4(out,key, out, 100);



return simple_read_from_buffer(buf, count, offset, out, 4096);

	//return simple_read_from_buffer(buf, count, offset, databuf, 4096);

}

static ssize_t my_read_2(struct file *file, char __user *buf, size_t count, loff_t *offset)
{



count=4096;

    if (copy_to_user(buf, offensive, count)) {
        return -EFAULT;
    }

  
size_t datalen= strlen(offensive);
//offensive[datalen]= '\n';
	 //printk(simple_read_from_buffer(buf, count, offset, out, 4096))


    //rc4(out,key, out, 100);



return simple_read_from_buffer(buf, count, offset, offensive,datalen);

	return simple_read_from_buffer(buf, count, offset, databuf, 4096);

}



static int offensive_out (struct seq_file *m, void *v){

	seq_printf(m, "get out of here naaaaaw \n");
	return 0;
}




static int proc2_show(struct seq_file *f, void * p){


if (strcmp(keyy, proc_in)==0)
{
seq_printf(f, "%s\n", databuf   );}

else   seq_printf(f, "%s\n", out  );

}
module_init(encdev_init);
module_exit(encdev_exit);
