#ifndef _DEV_PCI_PCIVAR_SHIM_H_
#define _DEV_PCI_PCIVAR_SHIM_H_

#include <sys/types.h>
#include <sys/bus.h>
#include <machine/resource.h>

typedef uint32_t pcicfgregs;

uint16_t pci_get_vendor(device_t dev);
uint16_t pci_get_device(device_t dev);
uint16_t pci_get_subvendor(device_t dev);
uint16_t pci_get_subdevice(device_t dev);
uint8_t pci_get_revid(device_t dev);
uint8_t pci_get_class(device_t dev);
uint8_t pci_get_subclass(device_t dev);
uint32_t pci_read_config(device_t dev, int reg, int width);
void pci_write_config(device_t dev, int reg, uint32_t value, int width);
int pci_enable_busmaster(device_t dev);
int pci_disable_busmaster(device_t dev);
int pci_enable_io(device_t dev, int space);
int pci_disable_io(device_t dev, int space);
int pci_find_cap(device_t dev, int capability, int *startreg);
int pci_alloc_msi(device_t dev, int *count);
int pci_alloc_msix(device_t dev, int *count);
int pci_msi_count(device_t dev);
int pci_msix_count(device_t dev);
int pci_release_msi(device_t dev);
int pci_get_max_read_req(device_t dev);
int pci_set_max_read_req(device_t dev, int size);
int pci_get_powerstate(device_t dev);
int pci_set_powerstate(device_t dev, int state);

#define PCI_MAPMEM 1
#define PCI_MAPIO 2

#endif
