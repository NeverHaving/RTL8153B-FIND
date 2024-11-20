#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <libusb-1.0/libusb.h>

enum RtlVersion {
    RTL_VER_UNKNOWN = 0,
    RTL_VER_01,
    RTL_VER_02,
    RTL_VER_03,
    RTL_VER_04,
    RTL_VER_05,
    RTL_VER_06,
    RTL_VER_07,
    RTL_VER_08,
    RTL_VER_09,
    RTL_VER_MAX
};

/* PLA_TCR1 */
#define VERSION_MASK		0x7cf0

#define VENDOR_ID 0x0BDA  // 替换为目标供应商 ID
#define PRODUCT_ID 0x8153  // 替换为目标产品 ID

int main() {
    libusb_context *ctx = NULL;
    libusb_device **devices;
    int device_count;

    // 初始化 libusb
    if (libusb_init(&ctx) < 0) {
        fprintf(stderr, "Failed to initialize libusb.\n");
        return -1;
    }

    // 获取 USB 设备列表
    device_count = libusb_get_device_list(ctx, &devices);
    if (device_count < 0) {
        fprintf(stderr, "Failed to get device list.\n");
        libusb_exit(ctx);
        return -1;
    }

    // 查找目标 USB 设备
    libusb_device_handle *handle = NULL;
    printf("device_count: %d\n", device_count);
    for (ssize_t i = 0; i < device_count; ++i) {
        libusb_device *device = devices[i];
        struct libusb_device_descriptor descriptor;

        // 获取设备描述符
        if (libusb_get_device_descriptor(device, &descriptor) < 0) {
            printf("libusb_get_device_descriptor < 0\n");
            continue;
        }
        printf("idVendor: 0x%04X idProduct: 0x%04X\n", descriptor.idVendor, descriptor.idProduct);
        // 检查供应商 ID 和产品 ID
        if (descriptor.idVendor == VENDOR_ID && descriptor.idProduct == PRODUCT_ID) {
            // 打开设备
            if (libusb_open(device, &handle) == 0) {
                printf("Device found and opened.\n");
                break;
            }
        }
    }

    // 释放设备列表
    libusb_free_device_list(devices, 1);

    if (handle == NULL) {
        fprintf(stderr, "Device not found.\n");
        libusb_exit(ctx);
        return -1;
    }

    // 进行控制传输
    uint32_t data; // 控制传输数据
    uint32_t ocpData;
    uint32_t version;

    // 发送控制传输 (使用 Vendor 请求)
    int result = libusb_control_transfer(handle,
                                         0xC0,
                                         0x05,  // 请求
                                         0xe610, // 值
                                         0x0100, // 索引
                                         (unsigned char *)(&data),
                                         sizeof(data),
                                         1000); // 超时 (毫秒)

    if (result < 0) {
        fprintf(stderr, "Control transfer failed: %s\n", libusb_error_name(result));
    } else {
        printf("Control transfer sent successfully.\n");
        printf("data : 0x%X\n", data);
        ocpData = (data >> 16) & VERSION_MASK;
        printf("ocpData = 0x%X \n", ocpData);
        switch (ocpData) {
        case 0x4c00:
            version = RTL_VER_01;
            break;
        case 0x4c10:
            version = RTL_VER_02;
            break;
        case 0x5c00:
            version = RTL_VER_03;
            break;
        case 0x5c10:
            version = RTL_VER_04;
            break;
        case 0x5c20:
            version = RTL_VER_05;
            break;
        case 0x5c30:
            version = RTL_VER_06;
            break;
        case 0x4800:
            version = RTL_VER_07;
            break;
        case 0x6000:
            version = RTL_VER_08;
            break;
        case 0x6010:
            version = RTL_VER_09;
            break;
        default:
            version = RTL_VER_UNKNOWN;
            printf("ERROR Unknown version 0x%04x\n", ocpData);
            break;
        }
        // printf("This RTL8153B Version = 0x%X \n", version);
        printf("\033[34mThis RTL8153B Version = 0x%X \n\033[0m", version);
    }

    // 关闭设备
    libusb_close(handle);
    libusb_exit(ctx);
    return 0;
}