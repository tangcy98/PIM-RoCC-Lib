#include "../src/PIMAPI.h"

using namespace PIMAPI;

int main()
{
    const PIMBasicInfo &info = PIMBasicInfo::getInstance();
    printf("-------- Unit Test -----------");

    printf("_scratchpad_size: \t %u\n", (u32)(info.params._scratchpad_size));
    printf("_subarr_per_mat: \t %u\n", (u32)(info.params._subarr_per_mat));
    printf("_mat_per_bank: \t %u\n", (u32)(info.params._mat_per_bank));
    printf("_banknums: \t %u\n", (u32)(info.params._banknums));
    printf("_inst_slots: \t %u\n", (u32)(info.params._inst_slots));
    printf("_subarrnums: \t %u\n", (u32)(info.params._subarrnums));
    printf("_usable_subarr: \t %u\n", (u32)(info.params._usable_subarr));
    printf("_ctrl_subarr_per_inst: \t %u\n", (u32)(info.params._ctrl_subarr_per_inst));
    printf("_subarr_rows: \t %u\n", (u32)(info.params._subarr_rows));
    printf("_subarr_cols: \t %u\n", (u32)(info.params._subarr_cols));
    printf("_ctrl_cols_stride: \t %u\n", (u32)(info.params._ctrl_cols_stride));
    printf("_max_addrspace: \t %u\n", (u32)(info.params._max_addrspace));
    return 0;
}