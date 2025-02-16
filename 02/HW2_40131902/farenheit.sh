read -p 'Enter Degree in Celsius : ' celsius
far=$(echo "scale=2; ($celsius * 9/5)+ 32 " | bc)
echo $far
