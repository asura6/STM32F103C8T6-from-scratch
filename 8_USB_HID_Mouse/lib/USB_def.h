/* Robin Isaksson 2017 */ 
#ifndef STM_USB_def
#define STM_USB_def

/* Endpoint values */
#define EP_BULK         (0x00U << 9U)
#define EP_CONTROL      (0x01U << 9U)
#define EP_ISO          (0x02U << 9U)
#define EP_INTERRUPT    (0x03U << 9U)
#define EP_DISABLED     0x00U
#define EP_STALL        0x01U
#define EP_NAK          0x02U
#define EP_VALID        0x03U
#define EP0             0x00U
#define EP1             0x01U
#define EP2             0x02U
#define EP3             0x03U
#define EP4             0x04U
#define EP5             0x05U
#define EP6             0x06U
#define EP7             0x07U

/* EndPoint Registers Bit Definitions */
#define EP_CTR_RX       0x8000      /* Correct RX Transfer */
#define EP_DTOG_RX      0x4000      /* RX Data Toggle */
#define EP_STAT_RX      0x3000      /* RX Status */
#define EP_SETUP        0x0800      /* EndPoint Setup */
#define EP_TYPE         0x0600      /* EndPoint Type */
#define EP_KIND         0x0100      /* EndPoint Kind */
#define EP_CTR_TX       0x0080      /* Correct TX Transfer */
#define EP_DTOG_TX      0x0040      /* TX Data Toggle */
#define EP_STAT_TX      0x0030      /* TX Status */
#define EP_EA           0x000F      /* EndPoint Address */

typedef struct {
    __IO uint16_t EP0R;
    uint16_t RESERVED0;
    __IO uint16_t EP1R;
    uint16_t RESERVED1;
    __IO uint16_t EP2R;
    uint16_t RESERVED2;
    __IO uint16_t EP3R;
    uint16_t RESERVED3;
    __IO uint16_t EP4R;
    uint16_t RESERVED4;
    __IO uint16_t EP5R;
    uint16_t RESERVED5;
    __IO uint16_t EP6R;
    uint16_t RESERVED6;
    __IO uint16_t EP7R;
    uint16_t RESERVED7;
    uint32_t RESERVED8[8];
    __IO uint16_t CNTR;
    uint16_t RESERVED9;
    __IO uint16_t ISTR;
    uint16_t RESERVED10;
    __IO uint16_t FNR;
    uint16_t RESERVED11;
    __IO uint16_t DADDR;
    uint16_t RESERVED12;
    __IO uint16_t BTABLE;
    uint16_t RESERVED13;
} USB_TypeDef;

#define USB_BASE        ((uint32_t)0x40005C00)
#define USB_EP(n)       (*((uint16_t *) (USB_BASE + 4*n)))
#define USB             ((USB_TypeDef *) USB_BASE)

typedef struct {
    __IO uint16_t ADDR_TX;
    uint16_t RESERVED0;
    __IO uint16_t COUNT_TX;
    uint16_t RESERVED1;
    __IO uint16_t ADDR_RX;
    uint16_t RESERVED2;
    __IO uint16_t COUNT_RX;
    uint16_t RESERVED3;
} BDT_TypeDef;

#define PMA_BASE        ((uint32_t)0x40006000) /* Buffer Descriptor Table */
#define USB_BDT(n)      ((BDT_TypeDef *) (PMA_BASE + 16UL*n))
#define USB0_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*0))
#define USB1_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*1))
#define USB2_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*2))
#define USB3_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*3))
#define USB4_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*4))
#define USB5_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*5))
#define USB6_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*6))
#define USB7_BDT        ((BDT_TypeDef *) (PMA_BASE + 16*7))

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_Setup_Packet_t;


/***********************************************************/
/*********   DESCRIPTOR STRUCTURE DEFINITIONS   ************/
/***********************************************************/

/* Device Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} USB_device_descriptor_t;

/* Configuration Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t wTotalLength0;      uint8_t wTotalLength1;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} USB_configuration_descriptor_t;

/* Interface Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} USB_interface_descriptor_t;

/* Endpoint Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSize0;    uint8_t wMaxPacketSize1;
    uint8_t bInterval;
} USB_endpoint_descriptor_t;

/* String Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t *bString;
} USB_string_descriptor_t;

/* HID Descriptor */
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType_HID;
    uint8_t bcdHID0;            uint8_t bcdHID1;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bDescriptorType_class;
    uint8_t wDescriptorLength0; uint8_t wDescriptorLength1;
} USB_HID_descriptor_t;

/* It is useful to combine many descriptors into a single contiguous block */
typedef struct {
    USB_configuration_descriptor_t conf_desc;
    USB_interface_descriptor_t interf_desc;
    USB_HID_descriptor_t HID_desc;
    USB_endpoint_descriptor_t endp_desc;
} USB_combined_descriptor_t;

#endif
