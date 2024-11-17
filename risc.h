#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>

namespace riscy::risc {

struct Instr {
  int opcode;

  virtual ~Instr() = default;

  Instr(int opcode) : opcode(opcode) {}

  virtual inline std::ostream &operator<<(std::ostream &os) {
    return os << "Instr{opcode=" << opcode << "}";
  }

  virtual inline std::string to_string() { return "???"; }
};

struct InstrR : public Instr {
  int rd, funct3, rs1, rs2, funct7;

  InstrR(int opcode, int rd, int funct3, int rs1, int rs2, int funct7)
      : Instr(opcode), rd(rd), funct3(funct3), rs1(rs1), rs2(rs2),
        funct7(funct7) {}

  inline std::ostream &operator<<(std::ostream &os) override {
    return os << "InstrR{opcode=" << opcode << ",rd=" << rd
              << ",funct3=" << funct3 << ",rs1=" << rs1 << ",rs2=" << rs2
              << ",funct7=" << funct7 << "}";
  }

  inline std::string to_string() override {
    switch (funct3) {
    case 0b000: // ADD,SUB,(MUL)
    {
      std::string oper = "<UNKNOWN OPERATOR>";
      switch (funct7) {
      case 0b0000000:
        oper = "+"; // ADD
        break;
      case 0b0100000: // SUB
        oper = "-";
        break;
      case 0b0000001: // MUL
        oper = "*";
        break;
      }
      return std::format("x{} = x{} {} x{}", rd, rs1, oper, rs2);
    }
    case 0b001: // SLL
    {
      switch (funct7) {
      case 0b0000000: // SLL
        return std::format("x{} = x{} << x{}", rd, rs1, rs2);
      }
      break;
    }
    case 0b010: // SLT
    {
      switch (funct7) {
      case 0b0000000: // SLT
        return std::format("x{} = (x{} < x{})", rd, rs1, rs2);
      }
      break;
    }
    case 0b011: // SLTU
    {
      switch (funct7) {
      case 0b0000000: // SLTU
        return std::format("x{} = ((unsigned)x{} < (unsigned)x{})", rd, rs1,
                           rs2);
      }
      break;
    }
    case 0b100: // XOR
    {
      switch (funct7) {
      case 0b0000000: // XOR
        return std::format("x{} = x{} ^ x{}", rd, rs1, rs2);
      }
      break;
    }
    case 0b101: // SRL,SRA
    {
      std::string oper = "<UNKNOWN OPERATOR>";
      switch (funct7) {
      case 0b0000000: // SRL
        oper = ">>";
        break;
      case 0b0100000: // SRA
        oper = ">>>"; // TODO: codegen will be different since no >>> in C
        break;
      }
      return std::format("x{} = x{} {} x{}", rd, rs1, oper, rs2);
    }
    case 0b110: // OR
    {
      switch (funct7) {
      case 0b0000000: // OR
        return std::format("x{} = x{} | x{}", rd, rs1, rs2);
      }
      break;
    }
    case 0b111: // AND
    {
      switch (funct7) {
      case 0b0000000: // AND
        return std::format("x{} = x{} & x{}", rd, rs1, rs2);
      }
      break;
    }
    }
    return "??? (fall-through)";
  }
};

struct InstrI : public Instr {
  int rd, funct3, rs1, imm;

  InstrI(int opcode, int rd, int funct3, int rs1, int imm)
      : Instr(opcode), rd(rd), funct3(funct3), rs1(rs1), imm(imm) {}

  inline std::ostream &operator<<(std::ostream &os) override {
    return os << "InstrI{opcode=" << opcode << ",rd=" << rd
              << ",funct3=" << funct3 << ",rs1=" << rs1 << ",imm=" << imm
              << "}";
  }

  inline std::string to_string() override {
    switch ((opcode >> 2) & 0b11111) {
    case 0b00000: // LOAD
    {
      std::string oper = "<UNKNOWN OPERATOR>";
      switch (funct3) {
      case 0b000: // LB
        oper = "LB";
        break;
      case 0b001: // LH
        oper = "LH";
        break;
      case 0b010: // LW
        oper = "LW";
        break;
      case 0b100: // LBU
        oper = "LBU";
        break;
      case 0b101: // LHU
        oper = "LHU";
        break;
      }
      return std::format("x{} = {}(x{} + {})", rd, oper, rs1, imm);
    }

    case 0b00100: // OP_IMM
    {
      std::string oper = "<UNKNOWN OPERATOR>";
      switch (funct3) {
      case 0b000: // ADDI
        oper = "+";
        break;
      case 0b010: // SLTI
        oper = "<";
        break;
      case 0b011: // SLTIU
        oper = "<";
        break;
      case 0b100: // XORI
        oper = "^";
        break;
      case 0b110: // ORI
        oper = "|";
        break;
      case 0b111: // ANDI
        oper = "&";
        break;
      case 0b001: // SLLI
        oper = "<<";
        break;
      case 0b101: // SRLI,SRAI
      {
        if ((imm & 0b100000) == 0) {
          oper = ">>";
        } else {
          oper = ">>>"; // TODO: codegen will be different since no >>> in C
        }
        break;
      }
      }
      return std::format("x{} = x{} {} {}", rd, rs1, oper, imm);
    }

    case 0b11001: // JALR
    {
      if (rd == 0 && rs1 == 1 && imm == 0) {
        return "return";
      }
      return std::format("x{} = pc + 4; pc = x{} + {}", rd, rs1, imm);
    }
    }

    switch (funct3) {
    case 0b000: // ADDI
      return std::format("x{} = x{} + {}", rd, rs1, imm);
    case 0b010: // SLTI
      return std::format("x{} = (x{} < {})", rd, rs1, imm);
    case 0b011: // SLTIU
      return std::format("x{} = ((unsigned)x{} < {})", rd, rs1, imm);
    case 0b100: // XORI
      return std::format("x{} = x{} ^ {}", rd, rs1, imm);
    case 0b110: // ORI
      return std::format("x{} = x{} | {}", rd, rs1, imm);
    case 0b111: // ANDI
      return std::format("x{} = x{} & {}", rd, rs1, imm);
    case 0b001: // SLLI
      return std::format("x{} = x{} << {}", rd, rs1, imm);
    case 0b101: // SRLI,SRAI
    {
      std::string oper = "<UNKNOWN OPERATOR>";
      if ((imm & 0b100000) == 0) {
        oper = ">>";
      } else {
        oper = ">>>"; // TODO: codegen will be different since no >>> in C
      }
      return std::format("x{} = x{} {} {}", rd, rs1, oper, imm & 0b11111);
    }
    }
    return "??? (fall-through)";
  }
};

struct InstrS : public Instr {
  // Note: imm is composed of two non-overlapping ranges
  // Note: imm can be split in B-type instructions
  int imm, funct3, rs1, rs2;

  InstrS(int opcode, int imm, int funct3, int rs1, int rs2)
      : Instr(opcode), imm(imm), funct3(funct3), rs1(rs1), rs2(rs2) {}

  inline std::ostream &operator<<(std::ostream &os) override {
    return os << "InstrS{opcode=" << opcode << ",imm=" << imm
              << ",funct3=" << funct3 << ",rs1=" << rs1 << ",rs2=" << rs2
              << "}";
  }
};

struct InstrU : public Instr {
  // Note: imm can be split in J-type instructions
  int rd, imm;

  InstrU(int opcode, int rd, int imm) : Instr(opcode), rd(rd), imm(imm) {}

  inline std::ostream &operator<<(std::ostream &os) override {
    return os << "InstrU{opcode=" << opcode << ",rd=" << rd << ",imm=" << imm
              << "}";
  }
};

enum InstrType : int {
  LOAD = 0,
  LOAD_FP,
  _custom_0,
  MISC_MEM,
  OP_IMM,
  AUIPC,
  OP_IMM_32,
  _invalid_48b_1,
  //
  STORE,
  STORE_FP,
  _custom_1,
  AMO,
  OP,
  LUI,
  OP_32,
  _invalid_64b,
  //
  MADD,
  MSUB,
  NMSUB,
  NMADD,
  OP_FP,
  _reserved_10101,
  _custom_2_rv128,
  _invalid_48b_2,
  //
  BRANCH,
  JALR,
  _reserved_11010,
  JAL,
  SYSTEM,
  _reserved_11101,
  _custom_3_rv128,
  _invalid_ge80b,
};

constexpr std::string_view InstrTypeNames[] = {
    "LOAD",
    "LOAD_FP",
    "_custom_0",
    "MISC_MEM",
    "OP_IMM",
    "AUIPC",
    "OP_IMM_32",
    "_invalid_48b_1",
    //
    "STORE",
    "STORE_FP",
    "_custom_1",
    "AMO",
    "OP",
    "LUI",
    "OP_32",
    "_invalid_64b",
    //
    "MADD",
    "MSUB",
    "NMSUB",
    "NMADD",
    "OP_FP",
    "_reserved_10101",
    "_custom_2_rv128",
    "_invalid_48b_2",
    //
    "BRANCH",
    "JALR",
    "_reserved_11010",
    "JAL",
    "SYSTEM",
    "_reserved_11101",
    "_custom_3_rv128",
    "_invalid_ge80b",
};

[[nodiscard]] inline std::shared_ptr<Instr> decode_instr(uint32_t n) {
  int opcode = n & 0b1111111;

  assert((opcode & 0b11) == 0b11);

  int tag = (opcode >> 2) & 0b11111;
  auto type = InstrTypeNames[tag];

  std::cout << "(type=" << type << ", tag=" << std::bitset<5>(tag) << ")\n";

  switch (tag) {
  case InstrType::LOAD:
  case InstrType::OP_IMM:
  case InstrType::JALR: {
    // I-type
    // imm[11:0] | rs1    | funct3 | rd    | opcode
    // 31-20       19-15    14-12    11-7    6-0
    int rd = (n >> 7) & 0b11111;
    int funct3 = (n >> 12) & 0b111;
    int rs1 = (n >> 15) & 0b11111;
    int imm = (n >> 20);
    // sign-extend
    if (imm & 0x800) {
      imm |= 0xFFFFF000;
    }
    return std::make_shared<InstrI>(opcode, rd, funct3, rs1, imm);
  }
  case InstrType::OP: {
    // R-type
    // funct7 | rs2    | rs1    | funct3 | rd    | opcode
    // 31-25    24-20    19-15    14-12    11-7    6-0
    int rd = (n >> 7) & 0b11111;
    int funct3 = (n >> 12) & 0b111;
    int rs1 = (n >> 15) & 0b11111;
    int rs2 = (n >> 20) & 0b11111;
    int funct7 = (n >> 25) & 0b1111111;
    return std::make_shared<InstrR>(opcode, rd, funct3, rs1, rs2, funct7);
  }
  case InstrType::STORE: {
    // S-type
    // imm[11:5] | rs2    | rs1    | funct3 | imm[4:0] | opcode
    // 31-25       24-20    19-15    14-12    11-7       6-0
    int imm = ((n >> 7) & 0b11111) | ((n >> 25) & 0b111111111111);
    int funct3 = (n >> 12) & 0b111;
    int rs1 = (n >> 15) & 0b11111;
    int rs2 = (n >> 20) & 0b11111;
    return std::make_shared<InstrS>(opcode, imm, funct3, rs1, rs2);
  }
  case InstrType::BRANCH: {
    // B-type
    // imm[12] | imm[10:5] | rs2    | rs1    | funct3 | imm[4:1] | imm[11] |
    // opcode
    // 31        30-25       24-20    19-15    14-12    11-8       7
    // 6-0
    int imm = ((n >> 7) & 0b111111) | ((n >> 25) & 0b111111111111) << 1;
    int funct3 = (n >> 12) & 0b111;
    int rs1 = (n >> 15) & 0b11111;
    int rs2 = (n >> 20) & 0b11111;
    return std::make_shared<InstrS>(opcode, imm, funct3, rs1, rs2);
  }
  case InstrType::LUI:
  case InstrType::AUIPC: {
    // U-type
    // imm[31:12] | rd    | opcode
    // 31-12        11-7    6-0
    int rd = (n >> 7) & 0b11111;
    int imm = (n >> 12) << 12;
    return std::make_shared<InstrU>(opcode, rd, imm);
  }
  case InstrType::JAL: {
    // J-type
    // imm[20] | imm[10:1] | imm[11] | imm[19:12] | rd    | opcode
    // 31        30-21       20        19-12        11-7    6-0
    int rd = (n >> 7) & 0b11111;
    int imm = ((n >> 21) & 0b11111111111) << 1;
    imm |= ((n >> 20) & 1) << 11;
    imm |= ((n >> 12) & 0b11111111) << 12;
    // sign-extend
    if (imm & 0x100000) {
      imm |= 0xFFF00000;
    }
    return std::make_shared<InstrU>(opcode, rd, imm);
  }
  default:
    return std::make_shared<Instr>(opcode);
  }
}

} // namespace riscy::risc
