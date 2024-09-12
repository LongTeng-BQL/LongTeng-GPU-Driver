/** @file LongTengGopDriverUefi.c

  The simple & naïve UEFI GOP driver for LongTeng-GPU project.
  Copyright (c) 2024 LongTeng-BQL Team. All rights granted.<BR>

  SPDX-License-Identifier: WTFPL

*/

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/DebugLib.h>


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
  DEBUG((DEBUG_INFO,"%a entry",__func__));
  return EFI_SUCCESS;
}
