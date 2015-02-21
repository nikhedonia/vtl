
define \n


endef

define nt

  
endef


rwildcard    =$(wildcard $1$2)$(foreach d,$(wildcard $1*), $(call rwildcard,$d/,$2))
uniq         =$(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

#-----------------------------------------------------------------------------------------------------

include config.m

inc         := include

CPP         :=$(filter $(CPP_FILTER),$(call rwildcard,*/,*))

SRC         :=$(filter %,$(CPP))
TESTSRC     :=$(filter test/%,$(CPP))
APPSRC      :=$(filter app/%,$(CPP))  $(TESTSRC)
LIBDIR      :=$(patsubst $(inc)/%/,%,$(wildcard $(inc)/*/))
libPath      =-Lbuild/$1/lib $(LIBPATHS)

getTar      =$(foreach b,$1,$(word 2,$(subst /, ,$(b))))
getType     =$(foreach b,$1,$(word 3,$(subst /, ,$(b))))
remprefix   =$(3:build/$1/$2/%=%)
cpp2dep     =$(foreach tar,$2,$(addprefix build/$(tar)/dep/,$(1:%=%.d)))
cpp2obj     =$(foreach tar,$2,$(addprefix build/$(tar)/obj/,$(1:%=%.o)))
obj2app     =$(patsubst %.cpp.o,%,$(subst obj,bin,$1))
toLog       =$(subst /$(call getType,$1)/,/log/,$1.log)

lib2ar      =$(foreach tar,$2,$(patsubst %,build/$(tar)/lib/lib%.a,$1))

nativepath  =$(call uniq,$(foreach cpp,$(1),$(inc)/$(word 2,$(subst /, ,$(dir $(cpp))))))
getobj      =$(filter build/$2/obj/$(word 4,$($1:/= )),$3) 
getlibname  =$(call uniq,$(foreach head,$(filter $(inc)/%,$1),$(word 2,$(subst /, ,$(head)))))
getlib      =$(call uniq,$(foreach head,$(filter $(inc)/%,$1),-l$(word 2,$(subst /, ,$(head)))))
cpp2ar      =$(foreach tar,$2,$(call lib2ar,$(call getlibname,$1),$(tar)))
libFilter   =$(filter src/$1/%,$2)
prepdir     =mkdir -p $(dir $1)

makeobj     =$(CXX) $($(call getTar,$1)) $(INCPATHS) -I$(call nativepath,$2) -o $1 -c $2
makedep     =$(CXX) $($(call getTar,$1)) $(INCPATHS) -I$(call nativepath,$2) -M -MT$(call cpp2obj,$2,$3) $2\
| sed s/:/:=/ > $1


DEP        :=$(call cpp2dep,$(CPP),$(TARGET))
OBJ        :=$(call cpp2obj,$(CPP),$(TARGET))
LIBOBJ     :=$(call cpp2obj,$(SRC),$(TARGET))
APPOBJ     :=$(call cpp2obj,$(APPSRC),$(TARGET))
TESTOBJ    :=$(call cpp2obj,$(TESTSRC),$(TARGET))
AR         :=$(call lib2ar,$(LIBDIR),$(TARGET))
APP        :=$(call obj2app,$(APPOBJ))
TEST       :=$(call obj2app,$(TESTOBJ))

$(info $(TEST))

makeDepRules=$(eval $(foreach tar,$(TARGET),$(foreach cpp,$(CPP),$(\n)$(call depRule,$(cpp),$(tar)))))
makeObjRules=$(eval $(foreach cpp,$(CPP),$(foreach tar,$(TARGET),$(\n)$(call objRule,$(cpp),$(tar)))))
makeLibRules=$(eval $(foreach lib,$(LIBDIR),$(foreach tar,$(TARGET),$(\n)$(call libRule,$(lib),$(tar),$(SRC)))))
makeBinRules=$(eval $(foreach appobj,$(APPOBJ),$(call binRule,$(\n)$(call obj2app,$(appobj)),$(appobj))))

define depRule 
$(call cpp2dep,$1,$2):$1
	$$(call prepdir,$$@) 
	$$(call makedep,$$@,$$<,$2) 
endef

define objRule
$(call cpp2obj,$1,$2):$1 
	$$(call prepdir,$$@)
	$$(call makeobj,$$@,$$<)
endef

define libRule
$(call lib2ar,$1,$2):$(call cpp2obj,$(call libFilter,$1,$3),$2) 
	$$(call prepdir,$$@)
	ar -rcvs $$@ $$+ 
endef

define binRule
$1:$2 $$(call cpp2ar,$$($2),$(call getTar,$1))
	$$(call prepdir,$$@)
	$$(CXX) $$(call libPath,$$(call getTar,$$<)) $$< $$(call getlib,$$($$<)) -o $$@ 
endef

define test2log
  echo "$(1):" >> $2
  $1 >> $2
endef

.PHONY: dep obj ar app 
all: dep obj ar app test.log

dep:$(DEP)
obj:$(OBJ)
ar:$(AR)
app:$(APP)  

clean:
	rm -rf build

test.log: $(TEST)
	date > test.log
	$(foreach test,$^,$(\n)$(call test2log,$(test),test.log))



include $(DEP)
$(call mapObj2Lib)
$(call makeDepRules)
$(call makeObjRules)
$(call makeLibRules)
$(call makeBinRules)









 





