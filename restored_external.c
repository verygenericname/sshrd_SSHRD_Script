/*
 *  ramdisk ssh server
 *
 *  Copyright (c) 2015 xerub
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOTypes.h>
#include "IOUSBDeviceControllerLib.h"

IOUSBDeviceDescriptionRef IOUSBDeviceDescriptionCreateWithType(CFAllocatorRef, CFStringRef);

char bootargs[2048];
size_t len = sizeof(bootargs) - 1;

io_service_t get_service(const char *name, unsigned int retry) {
    io_service_t service;
    CFDictionaryRef match = IOServiceMatching(name);

    while (1) {
        CFRetain(match);
        service = IOServiceGetMatchingService(kIOMasterPortDefault, match);
        if (service || !retry--) {
            break;
        }
        printf("Didn't find %s, trying again\n", name);
        sleep(1);
    }

    CFRelease(match);
    return service;
}

/* reversed from restored_external */
int init_usb(void) {
    int i;
    CFNumberRef n;
    io_service_t service;
    CFMutableDictionaryRef dict;
    IOUSBDeviceDescriptionRef desc;
    IOUSBDeviceControllerRef controller;

    desc = IOUSBDeviceDescriptionCreateWithType(kCFAllocatorDefault, CFSTR("standardMuxOnly")); /* standardRestore */
    if (!desc) {
        return -1;
    }
    IOUSBDeviceDescriptionSetSerialString(desc, CFSTR("SSHRD_Script " __DATE__ " " __TIME__ ));

    controller = 0;
    while (IOUSBDeviceControllerCreate(kCFAllocatorDefault, &controller)) {
        printf("Unable to get USB device controller\n");
        sleep(3);
    }
    IOUSBDeviceControllerSetDescription(controller, desc);

    CFRelease(desc);
    CFRelease(controller);

    service = get_service("AppleUSBDeviceMux", 10);
    if (!service) {
        return -1;
    }

    dict = CFDictionaryCreateMutable(NULL, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    i = 7;
    n = CFNumberCreate(NULL, kCFNumberIntType, &i);
    CFDictionarySetValue(dict, CFSTR("DebugLevel"), n);
    CFRelease(n);

    i = IORegistryEntrySetCFProperties(service, dict);
    CFRelease(dict);
    IOObjectRelease(service);

    return i;
}

#include "micro_inetd.c" /* I know, I am a bad person for doing this */
char *execve_params[] = { "micro_inetd", "22", "/usr/local/bin/dropbear", "-i", NULL };

/* chopped from https://code.google.com/p/iphone-dataprotection/ */
int main(int argc, char *argv[]) {
    printf("Starting ramdisk tool\n");
    printf("Compiled " __DATE__ " " __TIME__ "\n");

    io_service_t service = get_service("IOWatchDogTimer", 0);
    if (service) {
        int zero = 0;
        CFNumberRef n = CFNumberCreate(NULL, kCFNumberSInt32Type, &zero);
        IORegistryEntrySetCFProperties(service, n);
        CFRelease(n);
        IOObjectRelease(service);
    }

    if (init_usb()) {
        printf("USB init FAIL\n");
    } else {
        printf("USB init done\n");
    }
    
    if (sysctlbyname("kern.bootargs", bootargs, &len, NULL, 0) == 0) {
        if (strnstr(bootargs, "TrollStore", len)) {
            printf("waiting for 3 seconds to prevent issues\n");
            sleep(3);
            
            char mount_filesystems[50];
            char troll_install[50];
            char reboot[50];
            strcpy(mount_filesystems, "/bin/bash /usr/bin/mount_filesystems");
            strcpy(reboot, "/sbin/reboot");
            
            if (strnstr(bootargs, "TrollStore=Tips", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Tips");
            }
            if (strnstr(bootargs, "TrollStore=Books", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Books");
            }
            if (strnstr(bootargs, "TrollStore=VoiceMemos", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller VoiceMemos");
            }
            if (strnstr(bootargs, "TrollStore=Weather", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Weather");
            }
            if (strnstr(bootargs, "TrollStore=Files", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Files");
            }
            if (strnstr(bootargs, "TrollStore=Mail", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Mail");
            }
            if (strnstr(bootargs, "TrollStore=Home", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Home");
            }
            if (strnstr(bootargs, "TrollStore=Calculator", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Calculator");
            }
            if (strnstr(bootargs, "TrollStore=Stocks", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Stocks");
            }
            if (strnstr(bootargs, "TrollStore=Maps", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Maps");
            }
            if (strnstr(bootargs, "TrollStore=Measure", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Measure");
            }
            if (strnstr(bootargs, "TrollStore=Translate", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Translate");
            }
            if (strnstr(bootargs, "TrollStore=FaceTime", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller FaceTime");
            }
            if (strnstr(bootargs, "TrollStore=Contacts", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Contacts");
            }
            if (strnstr(bootargs, "TrollStore=Magnifier", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Magnifier");
            }
            if (strnstr(bootargs, "TrollStore=iTunes", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller iTunes");
            }
            if (strnstr(bootargs, "TrollStore=Shortcuts", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Shortcuts");
            }
            if (strnstr(bootargs, "TrollStore=TV", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller TV");
            }
            if (strnstr(bootargs, "TrollStore=Music", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Music");
            }
            if (strnstr(bootargs, "TrollStore=Notes", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Notes");
            }
            if (strnstr(bootargs, "TrollStore=Calendar", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Calendar");
            }
            if (strnstr(bootargs, "TrollStore=Reminders", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Reminders");
            }
            if (strnstr(bootargs, "TrollStore=Podcasts", len)) {
                strcpy(troll_install, "/bin/bash /usr/bin/trollstoreinstaller Podcasts");
            }
            printf("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQQQQQWQQQQQWWWBBBHHHHHHHHHBWWWQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQD!`__ssaaaaaaaaaass_ass_s____.  -~''??9VWQQQQQQQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQQQP'_wmQQQWWBWV?GwwwmmWQmwwwwwgmZUVVHAqwaaaac,'?9$QQQQQQQQQQQQQQ\n");
            printf("QQQQQQQQQQQW! aQWQQQQW?qw#TTSgwawwggywawwpY?T?TYTYTXmwwgZ$ma/-?4QQQQQQQQQQQ\n");
            printf("QQQQQQQQQQW' jQQQQWTqwDYauT9mmwwawww?WWWWQQQQQ@TT?TVTT9HQQQQQQw,-4QQQQQQQQQ\n");
            printf("QQQQQQQQQQ[ jQQQQQyWVw2$wWWQQQWWQWWWW7WQQQQQQQQPWWQQQWQQw7WQQQWWc)WWQQQQQQQ\n");
            printf("QQQQQQQQQf jQQQQQWWmWmmQWU???????9WWQmWQQQQQQQWjWQQQQQQQWQmQQQQWL 4QQQQQQQQ\n");
            printf("QQQQQQQP'.yQQQQQQQQQQQP'       <wa,.!4WQQQQQQQWdWP??!'??4WWQQQWQQc ?QWQQQQQ\n");
            printf("QQQQQP'_a.<aamQQQW!<yF '!` ..  '??$Qa 'WQQQWTVP'    '??' =QQmWWV?46/ ?QQQQQ\n");
            printf("QQQP'sdyWQP?!`.-'?46mQQQQQQT!mQQgaa. <wWQQWQaa _aawmWWQQQQQQQQQWP4a7g -WWQQ\n");
            printf("QQ[ j@mQP'adQQP4ga, -????' <jQQQQQWQQQQQQQQQWW;)WQWWWW9QQP?'`  -?QzQ7L ]QQQ\n");
            printf("QW jQkQ@ jWQQD'-?$QQQQQQQQQQQQQQQQQWWQWQQQWQQQc '4QQQQa   .QP4QQQQfWkl jQQQ\n");
            printf("QE ]QkQk $D?`  waa '?9WWQQQP??T?47`_aamQQQQQQWWQw,-?QWWQQQQQ`'QQQD\\Qf(.QWQ\n");
            printf("QQ,-Qm4Q/-QmQ6 'WWQma/  '??QQQQQQL 4W'- -?$QQQQWP`s,awT$QQQ@  'QW@?$:.yQQQQ\n");
            printf("QQm/-4wTQgQWQQ,  ?4WWk 4waac -???$waQQQQQQQQF??'<mWWWWWQW?^  ` ]6QQ' yQQQQQ\n");
            printf("QQQQw,-?QmWQQQQw  a,    ?QWWQQQw _.  '????9VWaamQWV???'  a j/  ]QQf jQQQQQQ\n");
            printf("QQQQQQw,'4QQQQQQm,-$Qa     ???4F jQQQQQwc <aaas _aaaaa 4QW ]E  )WQ`=QQQQQQQ\n");
            printf("QQQQQQWQ/ $QQQQQQQa ?H ]Wwa,     ???9WWWh dQWWW,=QWWU?  ?!     )WQ ]QQQQQQQ\n");
            printf("QQQQQQQQQc-QWQQQQQW6,  QWQWQQQk <c                             jWQ ]QQQQQQQ\n");
            printf("QQQQQQQQQQ,'$WQQWQQQQg,.'?QQQQ'.mQQQmaa,.,                . .; QWQ.]QQQQQQQ\n");
            printf("QQQQQQQQQWQa ?$WQQWQQQQQa,.'?( mQQQQQQW[:QQQQm[ ammF jy! j( } jQQQ(:QQQQQQQ\n");
            printf("QQQQQQQQQQWWma '9gw?9gdB?QQwa, -??T$WQQ;:QQQWQ ]WWD _Qf +?! _jQQQWf QQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQws 'Tqau?9maZ?WQmaas,,    --~-- ---  . _ssawmQQQQQQk 3QQQQWQ\n");
            printf("QQQQQQQQQQQQQQQQWQga,-?9mwad?1wdT9WQQQQQWVVTTYY?YTVWQQQQWWD5mQQPQQQ ]QQQQQQ\n");
            printf("QQQQQQQWQQQQQQQQQQQWQQwa,-??$QwadV}<wBHHVHWWBHHUWWBVTTTV5awBQQD6QQQ ]QQQQQQ\n");
            printf("QQQQQQQQQQQQQQQQQQQQQQWWQQga,-'9$WQQmmwwmBUUHTTVWBWQQQQWVT?96aQWQQQ ]QQQQQQ\n");
            printf("QQQQQQQQQQWQQQQWQQQQQQQQQQQWQQma,-?9$QQWWQQQQQQQWmQmmmmmQWQQQQWQQW(.yQQQQQW\n");
            printf("QQQQQQQQQQQQQWQQQQQQWQQQQQQQQQQQQQga%,.  -??9$QQQQQQQQQQQWQQWQQV? sWQQQQQQQ\n");
            printf("QQQQQQQQQWQQQQQQQQQQQQQQWQQQQQQQQQQQWQQQQmywaa,;~^'!???????!^`_saQWWQQQQQQQ\n");
            printf("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQWWWWQQQQQmwywwwwwwmQQWQQQQQQQQQQQ\n");
            printf("QQQQQQQWQQQWQQQQQQWQQQWQQQQQWQQQQQQQQQQQQQQQQWQQQQQWQQQWWWQQQQQQQQQQQQQQQWQ\n");
            printf("TrollStore by opa334\n");
            printf("SSHRD_Script by Nathan (verygenericname)\n");
            system(mount_filesystems);
            system(troll_install);
            
            printf("hopefully done, rebooting in 5 seconds...\n");
            sleep(5);
            system(reboot);
            return 0;
        }
    }
    
    printf("llllllllllllllllllllllllllllllllllllllllllllllllll\n");
    printf("llllllllllllllllllllllllllllllllllllllllllllllllll\n");
    printf("lllllc:;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;:clllll\n");
    printf("lllll,.                                    .,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll,      '::::,             .,::::.      ,lllll\n");
    printf("lllll,      ,llll;             .:llll'      ,lllll\n");
    printf("lllll,      ,llll;             .:llll'      ,lllll\n");
    printf("lllll,      ,llll;             .:llll'      ,lllll\n");
    printf("lllll,      ,llll;             .:llll'      ,lllll\n");
    printf("lllll,      ,cccc,             .;cccc'      ,lllll\n");
    printf("lllll,       ....               .....       ,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll,            .''''''''''''.            ,lllll\n");
    printf("lllll,            ,llllllllllll,            ,lllll\n");
    printf("lllll,            ,llllllllllll,            ,lllll\n");
    printf("lllll,            ..............            ,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll,                                      ,lllll\n");
    printf("lllll:'....................................':lllll\n");
    printf("llllllllllllllllllllllllllllllllllllllllllllllllll\n");
    printf("llllllllllllllllllllllllllllllllllllllllllllllllll\n");
    printf("llllllllllllllllllllllllllllllllllllllllllllllllll\n");
    printf("SSHRD_Script by Nathan (verygenericname)\n");

    printf("Running server\n");
    return main2(4, execve_params);
}
