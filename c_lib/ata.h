#ifndef ATA_H
#define ATA_H

#include <stddef.h>
#include <stdint.h>
#include "pic.h"

#define ATA_PRIMARY_IO_BASE 0x1F0
#define ATA_PRIMARY_CTRL_BASE 0x3F6
#define ATA_REG_DATA 0x1F0
#define ATA_REG_ERROR 0x1F1
#define ATA_REG_FEATURES 0x1F1
#define ATA_REG_SECTOR_COUNT 0x1F2
#define ATA_REG_LBA_LOW 0x1F3
#define ATA_REG_LBA_MID 0x1F4
#define ATA_REG_LBA_HIGH 0x1F5
#define ATA_REG_DRIVE 0x1F6
#define ATA_REG_STATUS 0x1F7
#define ATA_REG_COMMAND 0x1F7

#define ATA_SR_ERR 0x01
#define ATA_SR_DRQ 0x08
#define ATA_SR_BSY 0x80

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline int ata_wait(void) {
    for (int i = 0; i < 4; i++) {
        inb(ATA_REG_STATUS);
    }

    int timeout = 1000000;

    while (timeout-- > 0) {
        uint8_t status = inb(ATA_REG_STATUS);

        if (status & ATA_SR_BSY) continue;
        if (status & (ATA_SR_ERR | 0x20)) return -1;
        if (status & ATA_SR_DRQ) return 0;
    }

    return -1;
}

static int ata_wait_not_busy(void) {
    for (int i = 0; i < 4; i++) inb(ATA_REG_STATUS);
    int timeout = 1000000;
    while (timeout-- > 0) {
        if (!(inb(ATA_REG_STATUS) & ATA_SR_BSY)) return 0;
    }
    return -1;
}

static inline int ata_read_sector(uint32_t lba, void *buf) {
    if (buf == NULL) {
        return -1;
    }

    outb(ATA_REG_DRIVE, 0xF0 | ((lba >> 24) & 0x0F));
    outb(ATA_REG_SECTOR_COUNT, 1);
    outb(ATA_REG_LBA_LOW, (uint8_t)(lba & 0xFF));
    outb(ATA_REG_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_REG_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_REG_COMMAND, 0x20);

    if (ata_wait() != 0) {
        return -1;
    }

    uint16_t *words = (uint16_t *)buf;
    for (int i = 0; i < 256; i++) {
        words[i] = inw(ATA_REG_DATA);
    }

    return 0;
}

static inline int ata_write_sector(uint32_t lba, const void *buf) {
    if (buf == NULL) {
        return -1;
    }

    outb(ATA_REG_DRIVE, 0xF0 | ((lba >> 24) & 0x0F));

    for (int i = 0; i < 4; i++) {
        inb(ATA_REG_STATUS);
    }

    outb(ATA_REG_SECTOR_COUNT, 1);
    outb(ATA_REG_LBA_LOW, (uint8_t)(lba & 0xFF));
    outb(ATA_REG_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_REG_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_REG_COMMAND, 0x30);

    if (ata_wait() != 0) {
        return -1;
    }

    const uint16_t *words = (const uint16_t *)buf;
    for (int i = 0; i < 256; i++) {
        outw(ATA_REG_DATA, words[i]);
    }

    outb(ATA_REG_COMMAND, 0xE7);
    return ata_wait_not_busy();
}

#endif