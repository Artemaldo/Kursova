#include <ntddk.h>
#pragma warning (disable:4459)
#define IOCTL_INCREMENT_KEY_FREQUENCY CTL_CODE(FILE_DEVICE_KEYBOARD, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
#define IOCTL_GET_KEY_FREQUENCY CTL_CODE(FILE_DEVICE_KEYBOARD, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

PDEVICE_OBJECT DeviceObject = NULL;
ULONG KeyFrequency[128] = { 0 };

NTSTATUS Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DeviceIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID Unload(PDRIVER_OBJECT DriverObject);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\KeyboardDriver");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\??\\KeyboardDriver");
    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_KEYBOARD, 0, FALSE, &DeviceObject);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to create device\n"));
        return status;
    }
    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(DeviceObject);
        KdPrint(("Failed to create symbolic link\n"));
        return status;
    }
    DriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl;
    DriverObject->DriverUnload = Unload;
    DbgPrint(("Keyboard Driver Loaded Successfully\n"));
    return STATUS_SUCCESS;
}

NTSTATUS Create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS Close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_INCREMENT_KEY_FREQUENCY:
    {
        char key = *(char*)Irp->AssociatedIrp.SystemBuffer;
        if (key < 128)
        {
            KeyFrequency[key]++;
            Irp->IoStatus.Information = 0;
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_GET_KEY_FREQUENCY:
    {
        char key = *(char*)Irp->AssociatedIrp.SystemBuffer;
        if (key < 128)
        {
            ULONG frequency = KeyFrequency[key];
            if (irpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(ULONG))
            {
                *(ULONG*)Irp->AssociatedIrp.SystemBuffer = frequency;
                Irp->IoStatus.Information = sizeof(ULONG);
            }
            else
            {
                status = STATUS_BUFFER_TOO_SMALL;
            }
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    default:
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

VOID Unload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\??\\KeyboardDriver");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DeviceObject);
    DbgPrint(("Keyboard Driver Unloaded Successfully\n"));
}
