#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char **argv, char **env)
{
    int i;
    int a=0;
    int clk;

    Verilated::commandArgs(argc, argv);
    // init top verilog instance
    Vcounter *top = new Vcounter;
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC *tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd");

    // initialize vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("Lab 1: Counter");

    // initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->ld = 0;
    top->v = 0;

    // run simulation for many clock cycles
    for (i = 0; i < 600; i++){
        if(top->count == 9){
            if(a<3){
                top->en = 0;
                a++;
            }
            else{
                top->en = 1;
            }
        }
        // dump variables into VCD file and toggle clock
        for (clk = 0; clk < 2; clk++)
        {
            tfp->dump(2 * i + clk); // unit is in ps!!!
            top->clk = !top->clk;
            top->eval();
        }

        vbdPlot(int(top->count), 0, 255); 
        vbdCycle(i+1);

        top->rst = (i < 2) | (i == 15);
        top->en = vbdFlag();

        if (Verilated::gotFinish())
            exit(0);
    }

    // ++++ Send count value to Vbuddy
    vbdHex(4, (int(top->count) >> 16) & 0xF);
    vbdHex(3, (int(top->count) >> 8) & 0xF);
    vbdHex(2, (int(top->count) >> 4) & 0xF);
    vbdHex(1, int(top->count) & 0xF);
    vbdCycle(i+1);
    // ---- end of Vbuddy output section

    // change input stimuli
    top->rst = (i < 2) | (i == 15);
    top->en = vbdFlag();
    if (Verilated::gotFinish()) exit(0);

    vbdClose();  // ++++
    tfp->close();
    exit(0);
}