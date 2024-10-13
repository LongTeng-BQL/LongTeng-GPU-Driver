/** @file LongTengGopDriverUefi.c

  The simple & naïve UEFI GOP driver for LongTeng-GPU project.
  Copyright (c) 2024 LongTeng-BQL Team. All rights granted.<BR>

  SPDX-License-Identifier: WTFPL

*/

#include <Uefi.h>
#include <LongTengGopDriverName.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/PciIo.h>

UINT64 OriginalPciAttributes; // FIXME: DO NOT USE GLOBAL VARIABLE!
EFI_PCI_IO_PROTOCOL *PciIo;   // FIXME: DO NOT USE GLOBAL VARIABLE!

EFI_STATUS
EFIAPI
LongTengGopDriverSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
)
{
  EFI_PCI_IO_PROTOCOL *PciIo;

  EFI_STATUS Status = gBS->OpenProtocol (
    ControllerHandle,
    &gEfiPciIoProtocolGuid,
    (VOID **)&PciIo,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UINT16 PciVendorId = 0;
  Status = PciIo->Pci.Read (
    PciIo,
    EfiPciIoWidthUint16,
    PCI_VENDOR_ID_OFFSET,
    sizeof (PciVendorId),
    &PciVendorId
  );
  if (EFI_ERROR (Status)) {
    goto CleanUp;
  }
  if (PciVendorId != FixedPcdGet16(PcdLongTengBQLVendorID)) {
    Status = EFI_UNSUPPORTED;
  }

CleanUp:
  gBS->CloseProtocol (
    ControllerHandle,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    ControllerHandle
  );
  return Status;
}

EFI_STATUS
EFIAPI
LongTengGopDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
)
{
  EFI_STATUS Status = gBS->OpenProtocol (
    ControllerHandle,
    &gEfiPciIoProtocolGuid,
    (VOID **)&PciIo,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot open PCI Protocol:%r\n",Status));
    return Status;
  }
  DEBUG_CODE_BEGIN();
  UINTN Seg=0, Bus=0, Device=0, Func=0;
  Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Device, &Func);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot get PCI SS:BB:DD:FF :%r\n",Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO,"Starting LongTeng-GPU @ %p:%p:%p:%p\n", Seg, Bus, Device, Func));
  DEBUG_CODE_END();

  Status = PciIo->Attributes (
    PciIo,
    EfiPciIoAttributeOperationGet,
    0,
    &OriginalPciAttributes
  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot read default attributes:%r\n",Status));
    return Status;
  }

  UINT64 PciSupports;
  Status = PciIo->Attributes (
    PciIo,
    EfiPciIoAttributeOperationSupported,
    0,
    &PciSupports
  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot read supported attributes:%r\n",Status));
    return Status;
  }

  Status = PciIo->Attributes (
    PciIo,
    EfiPciIoAttributeOperationEnable,
    (PciSupports & EFI_PCI_DEVICE_ENABLE) |
    EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE,
    NULL
  );
  DEBUG_CODE_BEGIN();
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot set supported attributes:%r\n",Status));
  }
  // Test memory access.
  UINT64 TestValue = 0xDEADBEEFCAFEBABE;
    PciIo->Mem.Write (
    PciIo,
    EfiPciIoWidthUint64,
    0,
    0x20,
    1,
    &TestValue
  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot perform DMA write:%r\n",Status));
    return Status;
  }
  PciIo->Mem.Read (
    PciIo,
    EfiPciIoWidthUint64,
    0,
    0x20,
    1,
    &TestValue
  );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Cannot perform DMA read:%r\n",Status));
    return Status;
  }
  if (TestValue != 0xDEADBEEFCAFEBABE) {
    DEBUG((DEBUG_ERROR," Write-read value does not match or BAR0:%llx\n",TestValue));
  }
  DEBUG_CODE_END();

  return Status;
}

EFI_STATUS
EFIAPI
LongTengGopDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
)
{
  DEBUG((DEBUG_INFO,"%a entry\n",__func__));

  // TODO: Recover original attribute.
  gBS->CloseProtocol (
    ControllerHandle,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    ControllerHandle
  );
  return EFI_SUCCESS;
}

STATIC EFI_DRIVER_BINDING_PROTOCOL mLongTengGopDriverBinding = {
  .Start = LongTengGopDriverStart,
  .Stop  = LongTengGopDriverStop,
  .Supported = LongTengGopDriverSupported,
  .Version = 0x10
};

/**
  The main entry of LongTeng GOP Driver.
  @param ImageHandle The Image Handle that this image belongs to.
  @param SystemTable The UEFI System Table.

  @returns EFI_STATUS The initialization status.
*/
EFI_STATUS
EFIAPI
LongTengGopDriverEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  DEBUG((DEBUG_INFO,"%a entry\n",__func__));
  EFI_STATUS Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mLongTengGopDriverBinding,
             ImageHandle,
             &mLongTengGopName,
             &mLongTengGopName2
             );
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}
