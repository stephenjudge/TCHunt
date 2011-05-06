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

import os
import random

algos = ["AES", "Serpent", "Twofish", "AES-Twofish", "AES-Twofish-Serpent", "Serpent-AES", "Serpent-Twofish-AES", "Twofish-Serpent"]
hashes = ["RIPEMD-160", "SHA-512", "Whirlpool"]
filesystem = ["FAT"]

for x in xrange(1000):
    for a in algos:
        for h in hashes:
            for fs in filesystem:
                
                # Create Volume
                if os.system('truecrypt -t -c --volume-type=Normal --encryption=%s --hash=%s --filesystem=%s -k "" -p "123" --random-source=/dev/urandom --size=%s %s-%s.tc' %(a,h,fs,6291456,x,a+"-"+h+"-"+fs)) == 0:
                    pass
                else:
                    os.unlink("%s-%s-%s.tc" %(a,h,fs))

