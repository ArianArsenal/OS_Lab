#!/bin/bash


num1=$1
op=$2
num2=$3


case $op in
    +)
        result=$(echo "$num1 + $num2" | bc)
        ;;
    -)
        result=$(echo "$num1 - $num2" | bc)
        ;;
    \X)
        result=$(echo "$num1 * $num2" | bc)
        ;;
    /)
        #division by zero
        if [ $num2 -eq 0 ]; then
            echo "Error: Division by zero is not allowed."
            exit 1
        fi
        result=$(echo "scale=2; $num1 / $num2" | bc)
        ;;
    *)
        echo "Error: Unsupported operation. Use +, -, *, or /"
        exit 1
        ;;
esac

echo "Result: $result"
