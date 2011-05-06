#!/bin/bash

# Copyright 2007-2011 "16 Systems" legal@16systems.com.
# All Rights Reserved.

# This file is part of TCHunt.

# TCHunt is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TCHunt is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TCHunt. If not, see <http://www.gnu.org/licenses/>.

password="123"
count=0

while [ $count -lt 1000 ]
do

for algo in "AES" "Serpent" "Twofish" "AES-Twofish" "AES-Twofish-Serpent" "Serpent-AES" "Serpent-Twofish-AES" "Twofish-Serpent"
  do
    for hash in "RIPEMD-160" "SHA-512" "Whirlpool"
      do
        for size in "6291456"
          do
            #echo $algo $hash $size      
            truecrypt -t -c --volume-type=Normal --encryption=$algo --hash=$hash --filesystem=FAT -k "" -p $password  --random-source=/dev/urandom --size=$size $count-$algo-$hash-$size.tc
          done
      done
  done
  
count=$[$count+1]
echo $count
done

