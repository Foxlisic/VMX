// Сохранить 16-битный регистр
void VMX::cpu_put16(int reg_id, uint16_t w)
{
    switch (reg_id & 3) {

        case 0: C->bc = w; break;
        case 1: C->de = w; break;
        case 2:

            if      (prefix == 1) C->ix = w;
            else if (prefix == 2) C->iy = w;
            else                  C->hl = w;
            break;

        case 3: C->sp = w; break;
    }
}

uint16_t VMX::cpu_get16(int reg_id)
{
    switch (reg_id & 3) {

        case 0: return C->bc;
        case 1: return C->de;
        case 2:

            if      (prefix == 1) return C->ix;
            else if (prefix == 2) return C->iy;
            else                  return C->hl;
            break;

        case 3: return C->sp;
    }

    return 0;
}

// cond=0-7
int VMX::cpu_condition(int cond)
{
    switch (cond & 7) {

        case 0: return (C->af & ZF) ? 0 : 1; // NZ
        case 1: return (C->af & ZF) ? 1 : 0; // Z
        case 2: return (C->af & CF) ? 0 : 1;
        case 3: return (C->af & CF) ? 1 : 0;
        case 4: return (C->af & PF) ? 0 : 1;
        case 5: return (C->af & PF) ? 1 : 0;
        case 6: return (C->af & SF) ? 0 : 1;
        case 7: return (C->af & SF) ? 1 : 0;
    }

    return 0;
}

// Получение значения 8 бит из регистра-памяти
uint8_t VMX::cpu_get8(int reg_id)
{
    switch (reg_id & 7)
    {
        case 0: return C->bc >> 8;
        case 1: return C->bc;
        case 2: return C->de >> 8;
        case 3: return C->de;
        case 4:

            if (prefix == 0) return C->hl >> 8;
            if (prefix == 1) return C->ix >> 8;
            if (prefix == 2) return C->iy >> 8;
            break;

        case 5:

            if (prefix == 0) return C->hl;
            if (prefix == 1) return C->ix;
            if (prefix == 2) return C->iy;
            break;

        case 6:

            if (prefix == 0) return read(C->hl);
            if (prefix == 1) return read(C->ix + C->d);
            if (prefix == 2) return read(C->iy + C->d);
            break;

        case 7:

            return C->af >> 8;
    }

    return 0;
}

// Сохранить в регистр или HL, IX+d, IY+d
void VMX::cpu_put8(int reg_id, uint8_t d)
{
    switch (reg_id & 7)
    {
        case 0: C->bc = (C->bc & 0x00FF) | (d << 8); break;
        case 1: C->bc = (C->bc & 0xFF00) | d;        break;
        case 2: C->de = (C->de & 0x00FF) | (d << 8); break;
        case 3: C->de = (C->de & 0xFF00) | d;        break;
        case 4:

            if (prefix == 0) C->hl = (C->hl & 0x00FF) | (d << 8);
            if (prefix == 1) C->ix = (C->ix & 0x00FF) | (d << 8);
            if (prefix == 2) C->iy = (C->ix & 0x00FF) | (d << 8);
            break;

        case 5:

            if (prefix == 0) C->hl = (C->hl & 0xFF00) | d;
            if (prefix == 1) C->ix = (C->ix & 0xFF00) | d;
            if (prefix == 2) C->iy = (C->ix & 0xFF00) | d;
            break;

        case 6:

            if (prefix == 0) write(C->hl, d);
            if (prefix == 1) write(C->ix + C->d, d);
            if (prefix == 2) write(C->iy + C->d, d);
            break;

        case 7:

            C->af = (C->af & 0x00FF) | (d << 8);
            break;
    }
}

// Обновить флаги 5 и 3
void VMX::cpu_update53(uint8_t data)
{
    C->af = (C->af & ~F5F) | (data & F5F);
    C->af = (C->af & ~F3F) | (data & F3F);
}

// Вычислить Parity и записать
void VMX::cpu_setpf(uint8_t a)
{
    a = (a >> 4) ^ a;
    a = (a >> 2) ^ a;
    a = (a >> 1) ^ a;
    C->af = (C->af & ~PF) | (a & 1 ? 0 : 1);
}

// Set Carry Flag
void VMX::cpu_setcf(uint8_t data)
{
    C->af = (C->af & ~CF) | (data ? CF : 0);
}

// Set Half Carry Flag
void VMX::cpu_sethf(uint8_t data)
{
    C->af = (C->af & ~HF) | (data ? HF : 0);
}

