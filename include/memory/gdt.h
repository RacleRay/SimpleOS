#ifndef __GDT_H
#define __GDT_H

#include "common/types.h"

/* GDT 全局描述符表: 地址寻址过程中，保存段地址和访问控制信息的结构。
       段地址 + 段内偏移 找到目标地址 */
class GlobalDescriptorTable {
public:
    class SegmentDescriptor {
    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
        uint32_t Base();
        uint32_t Limit();

    private:
        uint16_t limit_lo;
        uint16_t base_lo;
        uint8_t  base_hi;
        uint8_t  type;
        uint8_t  flags_limit_hi;
        uint8_t  base_vhi;
    } __attribute__((packed));  // 关闭内存对齐

    SegmentDescriptor nullSegmentDescriptor;
    SegmentDescriptor unusedSegmentDescriptor;
    SegmentDescriptor codeSegmentDescriptor;
    SegmentDescriptor dataSegmentDescriptor;

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t CodeSegmentSelector();
    uint16_t DataSegmentSelector();
};

#endif