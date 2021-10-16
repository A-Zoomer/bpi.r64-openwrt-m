static int
mt76_get_eeprom_file(struct mt76_dev *dev, int len)
{
	char path[64]="";
	struct file *fp;
	loff_t pos=0;
	int ret;
	struct inode *inode = NULL;
	loff_t size;

	//find calibration data in eeprom (searching for mt7622-wmac_rf.bin)
	ret = snprintf(path,sizeof(path),"/lib/firmware/mediatek/%s_rf.bin",dev->dev->driver->name);

	//controls
	if(ret<0)
		return -EINVAL;
	dev_info(dev->dev,"Load eeprom: %s\n",path);
	fp = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		dev_info(dev->dev,"Open eeprom file failed: %s\n",path);
		return -ENOENT;
	}

	inode = file_inode(fp);
	if ((!S_ISREG(inode->i_mode) && !S_ISBLK(inode->i_mode))) {
		printk(KERN_ALERT "invalid file type: %s\n", path);
		return -ENOENT;
	}
	size = i_size_read(inode->i_mapping->host);
	if (size < 0)
	{
		printk(KERN_ALERT "failed getting size of %s size:%lld \n",path,size);
		return -ENOENT;
	}

	//Calling kernel_read to register the eeprom file
	ret = kernel_read(fp, dev->eeprom.data, len, &pos);
	if(ret < size){
		dev_info(dev->dev,"Load eeprom ERR, count %d byte (len:%d)\n",ret,len);
		return -ENOENT;
	}
	filp_close(fp, 0);
	dev_info(dev->dev,"Load eeprom OK, count %d byte\n",ret);

	return 0;
}

