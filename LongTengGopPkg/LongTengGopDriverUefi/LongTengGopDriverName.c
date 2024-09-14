/** @file LongTengGopDriverName.c

  The driver name of LongTeng-GPU GOP Driver.
  Copyright (c) 2024 LongTeng-BQL Team. All rights granted.<BR>

  SPDX-License-Identifier: WTFPL

*/

#include <Uefi.h>
#include <LongTengGopDriverName.h>

EFI_STATUS
EFIAPI
LongTengGopDriverGetControllerName
(
  IN  EFI_COMPONENT_NAME_PROTOCOL                             *This,
  IN  EFI_HANDLE                                               ControllerHandle,
  IN  EFI_HANDLE                                               ChildHandle        OPTIONAL,
  IN  CHAR8                                                   *Language,
  OUT CHAR16                                                 **ControllerName
)
{
  if(ControllerName == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *ControllerName = L"LongTeng-GPU by LongTeng-BQL Team";
  (VOID)This;
  (VOID)ControllerHandle;
  (VOID)ChildHandle;
  (VOID)Language;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LongTengGopDriverGetDriverName
(
  IN EFI_COMPONENT_NAME_PROTOCOL           *This,
  IN  CHAR8                                *Language,
  OUT CHAR16                               **DriverName
)
{
  if(DriverName == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *DriverName = L"LongTengGopDriver";
  (VOID)This;
  (VOID)Language;
  return EFI_SUCCESS;
}



CONST EFI_COMPONENT_NAME_PROTOCOL mLongTengGopName = {
    .SupportedLanguages = "en",
    .GetControllerName = LongTengGopDriverGetControllerName,
    .GetDriverName = LongTengGopDriverGetDriverName
};

CONST EFI_COMPONENT_NAME2_PROTOCOL mLongTengGopName2 = {
    .SupportedLanguages = "en",
    .GetControllerName = (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)LongTengGopDriverGetControllerName,
    .GetDriverName = (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)LongTengGopDriverGetDriverName
};
