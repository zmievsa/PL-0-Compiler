#!/bin/bash
make compiler

for i in {1..35}
do
   echo "input$i"
   ./compile.out -a -v -l io/in/input$i.txt &> io/out/output$i.txt
done