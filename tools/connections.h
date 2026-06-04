#pragma once

#define dump_props
#define dump_tags_attrs

#include <set>
#include "nitrohost.h"

#ifdef __APPLE__

int listPorts(utf8 &result){
	return -1;
}

#endif

#ifdef __linux__

#include <libudev.h>

std::string pullEntry(udev_list_entry *prop,const std::string keyname){
	while(prop){
		const char *name=udev_list_entry_get_name(prop);
		if(name==keyname){
			const char *value=udev_list_entry_get_value(prop);
			if(!value) value="<none>";
//			std::cout << name << ":" << value << std::endl;
			return value;
		}
		prop = udev_list_entry_get_next(prop);
	}
	return "";
}

void dumpEntries(const std::string title,udev_list_entry *prop){
	std::cout << "### " << title << std::endl;
	while(prop){
		const char *value=udev_list_entry_get_value(prop);
		if(!value) value="<none>";
		std::cout << udev_list_entry_get_name(prop) << ":" << value << std::endl;
		prop = udev_list_entry_get_next(prop);
	}
}

int openPort(utf8 name,utf8 &result){
	std::cout << "openPort" << name << std::endl;
	result="\"connection_key\"";
	return 0;
}

int listPorts(utf8 &result){
	std::cout << "listPorts" << std::endl;
	std::stringstream ss;
	ss << "[ ";
	bool first = true;
	std::set<std::string> subsystems;
	udev *udev;
	udev_enumerate *it = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(it,"tty");    //"snd_seq" "sound" "pnp" "tty" "serio" 
	udev_enumerate_add_match_property(it,"ID_VENDOR","Raspberry_Pi");
	udev_enumerate_scan_devices(it);
	udev_list_entry *devices = udev_enumerate_get_list_entry(it);
	udev_list_entry *entry;
	udev_list_entry_foreach(entry, devices) {
		const char* name = udev_list_entry_get_name(entry);
		udev_device *dev = udev_device_new_from_syspath(udev, name);
		const char *subsystem = udev_device_get_subsystem(dev);
		const char *devtype = udev_device_get_devtype(dev);
		if(!devtype) devtype="<none>";
		const char *sysname= udev_device_get_sysname(dev);
		const char *vendor = udev_device_get_property_value(dev,"ID_VENDOR");
		if(!vendor) vendor="unknown";
		subsystems.insert(subsystem);
//		std::cout << "#?#" << subsystem << " - " << name << " is " << subsystem <<" sysname " << sysname << " vendor " << vendor << std::endl;
		udev_list_entry *props = udev_device_get_properties_list_entry(dev);

		const std::string devname=pullEntry(props,"DEVNAME");
		const std::string model=pullEntry(props,"ID_MODEL");
		const std::string serial=pullEntry(props,"ID_USB_SERIAL");
//		std::cout << "model:" << model << " serial:" << serial << std::endl;

		if (!first) ss << ",";
		first = false;
		ss << "{\"subsystem\":\"" << subsystem 
			<< "\",\"name\":\"" << devname 
			<< "\",\"id\":\"" << serial 
			<< "\",\"description\":\"" << "USB Serial Port" 			
			<< "\"}";	

#ifdef dump_props
		dumpEntries("props",props);
#endif
#ifdef dump_tags_attrs
		udev_list_entry *tags = udev_device_get_tags_list_entry(dev);
		dumpEntries("tags",tags);
		udev_list_entry *attrs = udev_device_get_sysattr_list_entry(dev);
		dumpEntries("attrs",attrs);
#endif
		udev_device_unref(dev);
	}
	udev_enumerate_unref(it);
	ss<<"]";
	result=ss.str();
	std::cout << result << std::endl;
	return 0;
}

#endif

/*

#include <libudev.h>
#include <stdio.h>

static struct udev_device*
get_child(
	 struct udev* udev, struct udev_device* parent, const char* subsystem)
{
  struct udev_device* child = NULL;
  struct udev_enumerate *enumerate = udev_enumerate_new(udev);

  udev_enumerate_add_match_parent(enumerate, parent);
  udev_enumerate_add_match_subsystem(enumerate, subsystem);
  udev_enumerate_scan_devices(enumerate);

  struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
  struct udev_list_entry *entry;

  udev_list_entry_foreach(entry, devices) {
	const char *path = udev_list_entry_get_name(entry);
	child = udev_device_new_from_syspath(udev, path);
	break;
  }

  udev_enumerate_unref(enumerate);
  return child;
}

static void enumerate_usb_mass_storage(struct udev* udev)
{
  struct udev_enumerate* enumerate = udev_enumerate_new(udev);

  udev_enumerate_add_match_subsystem(enumerate, "scsi");
  udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
  udev_enumerate_scan_devices(enumerate);

  struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
  struct udev_list_entry *entry;

  udev_list_entry_foreach(entry, devices) {
	const char* path = udev_list_entry_get_name(entry);
	struct udev_device* scsi = udev_device_new_from_syspath(udev, path);

	struct udev_device* block = get_child(udev, scsi, "block");
	struct udev_device* scsi_disk = get_child(udev, scsi, "scsi_disk");

	struct udev_device* usb
	  = udev_device_get_parent_with_subsystem_devtype(
		  scsi, "usb", "usb_device");

	if (block && scsi_disk && usb) {
		printf("block = %s, usb = %s:%s, scsi = %s\n",
		  udev_device_get_devnode(block),
		  udev_device_get_sysattr_value(usb, "idVendor"),
		  udev_device_get_sysattr_value(usb, "idProduct"),
		  udev_device_get_sysattr_value(scsi, "vendor"));
	}

	if (block)
	  udev_device_unref(block);

	if (scsi_disk)
	  udev_device_unref(scsi_disk);

	udev_device_unref(scsi);
  }

  udev_enumerate_unref(enumerate);
}

int main()
{
  struct udev* udev = udev_new();

  enumerate_usb_mass_storage(udev);

  udev_unref(udev);
  return 0;
}
*/