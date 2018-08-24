//
// Created by yangning on 18-1-1.
//
// Descriprion :
//
// Copyright (c) yangning All rights reserved.
//
#include "benckmark.h"
#include "your_protocol.h"
extern struct Option gOption;
int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        Option::printDefautOption();
    }
    else
    {
        Option::process_parameters(argc, argv);
        gOption.printCurrentOption();
    }

    BenchMark benchMark;

    benchMark.setWriteCallBack(writeCallBack);

    benchMark.setReadCallBack(readCallBack);

    benchMark.run();
    return 0;
}
