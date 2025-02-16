read -p 'Enter a Number :' number

if [ $number -le 0 ];then
echo "The Weather is Freezing"

elif [ $number -gt 0 ] && [ $number -lt 30 ];then
echo "The Weather is Cool"

else 
echo "The Weather is Hot"
fi
