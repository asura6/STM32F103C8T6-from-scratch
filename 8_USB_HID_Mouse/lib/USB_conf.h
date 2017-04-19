#ifndef STM_USB_conf
#define STM_USB_conf

#include "USB_def.h"

/******************************************/
/*********   USER CONFIGURATION   *********/
/******************************************/


// BEFORE USE:
// set THE USB_VID and USB_PID 
#define USB_VID 0x0000
#define USB_PID 0x0000

#define USB_MANUFACTURER_NAME 'R','a','i','n','F','o','o'
#define USB_MANUFACTURER_LEN 7U

#define USB_PRODUCT_NAME 'M','o','u','s','e',' ','t','e','s','t'
#define USB_PRODUCT_LEN 10U

#define USB_SERIAL '0','0','0','1'
#define USB_SERIAL_LEN 4U

/***********************************/
/*********   DESCRIPTORS   *********/
/***********************************/

static const USB_device_descriptor_t USB_device_descriptor = {
    .bLength            = 18U,
    .bDescriptorType    = 0x01U,
    .bcdUSB             = 0x0200U,
    .bDeviceClass       = 0x00U,
    .bDeviceSubClass    = 0x00U,
    .bDeviceProtocol    = 0x00U,
    .bMaxPacketSize     = 64U,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01U,
    .iProduct           = 0x02U,
    .iSerialNumber      = 0x03U,
    .bNumConfigurations = 0x01U
};

static const USB_combined_descriptor_t USB_combined_descriptor = {
    .conf_desc = {
        .bLength            = 0x09U,
        .bDescriptorType    = 0x02U,
        .wTotalLength0      = 34U,      .wTotalLength1  = 0x00,
        .bNumInterfaces     = 0x01U,
        .bConfigurationValue = 0x01U,
        .iConfiguration     = 0x00U,
        .bmAttributes       = 0x80U,
        .bMaxPower          = 50U,
    },
    .interf_desc = {
        .bLength            = 0x09U,
        .bDescriptorType    = 0x04U,
        .bInterfaceNumber   = 0x00U,
        .bAlternateSetting  = 0x00U,
        .bNumEndpoints      = 0x01U, //Endpoint zero is excluded
        .bInterfaceClass    = 0x03U,
        .bInterfaceSubClass = 0x00U,
        .bInterfaceProtocol = 0x02U,
        .iInterface         = 0x00U
    },
    .HID_desc = {
        .bLength            = 0x09U,
        .bDescriptorType_HID = 0x21U,
        .bcdHID0            = 0x01U,    .bcdHID1        = 0x01U,
        .bCountryCode       = 0x00U,
        .bNumDescriptors    = 0x01U,
        .bDescriptorType_class  = 0x22U,
        .wDescriptorLength0 = 50U,      .wDescriptorLength1  = 0x00U
    },
    .endp_desc = {
        .bLength            = 7U,
        .bDescriptorType    = 0x05U,
        .bEndpointAddress   = 0x01U | 0x80U,   //Address 1, direction IN
        .bmAttributes       = 0x03U,           //Interrupt endpoint
        .wMaxPacketSize0    = 64U,    .wMaxPacketSize1      = 0x00U,
        .bInterval          = 10U              //ms polling interval
    }
};

/*************************************************/
/*********   HID REPORT AND DESCRIPTOR   *********/
/*************************************************/

static const uint8_t HID_report_desc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

#define HID_REPORT_SIZE 3U

typedef struct {
    struct {
        uint8_t button1 : 1;
        uint8_t button2 : 1;
        uint8_t button3 : 1;
        uint8_t reserved : 5;
    } buttons;
    int8_t x;
    int8_t y;
} HID_report_t;

/******************************************/
/*********   STRING DESCRIPTORS   *********/
/******************************************/

static uint16_t USB_string_language[] = {0x0409U}; //English - United States
static uint16_t USB_string_manufacturer[] = {USB_MANUFACTURER_NAME};
static uint16_t USB_string_product[] = {USB_PRODUCT_NAME};
static uint16_t USB_string_serial[] = {USB_SERIAL};

static const USB_string_descriptor_t USB_string_descriptor_zero = {
    .bLength            = 4U,
    .bDescriptorType    = 0x03,
    .bString            = USB_string_language
};

static const USB_string_descriptor_t USB_string_descriptor_manufacturer = {
    .bLength            = 2U + (USB_MANUFACTURER_LEN << 1),
    .bDescriptorType    = 0x03,
    .bString            = USB_string_manufacturer
};

static const USB_string_descriptor_t USB_string_descriptor_product = {
    .bLength            = 2U + (USB_PRODUCT_LEN << 1),
    .bDescriptorType    = 0x03,
    .bString            = USB_string_product
};

static const USB_string_descriptor_t USB_string_descriptor_serial = {
    .bLength            = 2U + (USB_SERIAL_LEN << 1),
    .bDescriptorType    = 0x03,
    .bString            = USB_string_serial
};

#endif
