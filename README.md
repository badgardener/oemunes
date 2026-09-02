<p align="center">
  <a href="https://github.com/badgardener/oemunes">
    <img src="https://img.shields.io/static/v1.svg?style=for-the-badge&label=Project&message=OEMUNES%20%E2%80%94%20NES%20Emulator&labelColor=1e1e2e&color=f5e0dc" alt="OEMUNES — NES Emulator">
  </a>
</p>

<p align="center">
  <a href="https://github.com/badgardener/oemunes">
    <img src="https://img.shields.io/static/v1.svg?style=for-the-badge&label=Status&message=Work%20in%20Progress&labelColor=1e1e2e&color=fe640b" alt="Status: Work in Progress">
  </a>
  <a href="https://github.com/badgardener/oemunes">
    <img src="https://img.shields.io/static/v1.svg?style=for-the-badge&label=Language&message=C%20and%20C%2B%2B&labelColor=1e1e2e&color=cba6f7" alt="Language: C / C++">
  </a>
</p>

<p align="center">
  A lightweight, modular NES emulator core written in C and C++
</p>

---

# OEMUNES

**OEMUNES** is a lightweight Nintendo Entertainment System emulator **core** written primarily in **C**, with some **C++** where appropriate.

The project is being built from the ground up with a focus on:

- Accurate NES hardware behavior
- Clean and modular architecture
- Minimal dependencies
- Portable core components
- Cartridge and mapper abstraction
- Cycle-aware CPU/PPU/APU design
- Easy integration into other applications

> **OEMUNES currently contains only the emulator core.**
> It has **no graphical, audio, or other frontend**. Frontends can be built separately and communicate with the core through its API.

The project is actively under development, with hardware accuracy taking priority over rushing toward a playable emulator.

---

## Development Status

### Core Components

| Component  | Status                                       |
| ---------- | -------------------------------------------- |
| Memory Bus | **Done**                                     |
| Cartridge  | **Done**                                     |
| CPU        | **Architecturally Done — Opcodes Remaining** |
| PPU        | **Skeleton**                                 |
| APU        | **Not Started**                              |

### Mappers

| Mapper       | Status      |
| ------------ | ----------- |
| 0000 — NROM  | **Done**    |
| 0001 — MMC1  | **Planned** |
| 0002 — UxROM | **Planned** |

More mappers will be added as development progresses.

## Design Goals

### Accuracy over shortcuts

Emulate NES hardware behavior rather than relying on game-specific shortcuts.

### Modular hardware

CPU, PPU, APU, cartridge, mapper, controllers, and memory remain independently testable.

### Minimal dependencies

The emulator core should remain lightweight and independent of graphics or multimedia frameworks.

### Frontend independent

OEMUNES does **NOT** provide a frontend. Graphics, audio, input, and windowing can be implemented by applications using the core.

### Portable

The core should be usable from desktop, mobile, and other applications.

### Testable

Hardware behavior should be testable without requiring a graphical interface.

---

## Contributing

OEMUNES is currently a work in progress.

Bug reports, test cases, hardware documentation, and suggestions are welcome.

When reporting an issue, include:

- ROM/test information
- Expected behavior
- Actual behavior
- Relevant logs
- Reproduction steps

---

<p align="center">
  Built with C, C++, and a lot of curiosity about old hardware.
</p>

<p align="center">
  <a href="https://github.com/badgardener/oemunes">
    <img src="https://img.shields.io/static/v1.svg?style=for-the-badge&label=License&message=Apache%202.0&labelColor=1e1e2e&color=b4befe" alt="Apache 2.0">
  </a>
</p>
