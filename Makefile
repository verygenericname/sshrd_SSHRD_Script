PROJECT  = restored_external

CC       = xcrun -sdk iphoneos clang
CC_FLAGS = -arch arm64 -O3 -fvisibility=hidden -fvisibility-inlines-hidden 
$(CFLAGS)
LD_FLAGS = -framework CoreFoundation -framework IOKit -miphoneos-version-min=12.0 $(LDFLAGS)
STRIP    = xcrun -sdk iphoneos strip

.PHONY: clean

$(PROJECT): $(PROJECT).c
	$(CC) $(CC_FLAGS) $^ $(LD_FLAGS) -o $@
	$(STRIP) $@

clean:
	rm -rf *.o *.dSYM $(PROJECT)
