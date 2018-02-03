#ifndef	_MLFdatum_h_
#define	_MLFdatum_h_
#include <Rtypes.h>
#include <arpa/inet.h>
class MLFdatum {
private:
  union {                          //fBuf[8]=fVal=fVal32[2]
	UChar_t fBuf[8];
	ULong64_t fVal;
	UInt_t  fVal32[2];
	struct {
	    UChar_t fHeader;
	    UChar_t fReserved;
	    UShort_t fModule;
	    UShort_t fCh;
	    UShort_t fTime;
        };
  };
public:
    void Set64(ULong64_t val) {
	fVal = val;
    };
    void Set32(UInt_t val) {
	fVal = 0;
	fHeader = 0x20;
	fVal32[1] = val;
    };
    void Set(UShort_t val) {
	fVal = 0;
	fHeader = 0x20;
	fTime = htons(val);
    };
    void Set(UShort_t mod, UShort_t ch, UShort_t time) {
	fVal = 0;
	fHeader = 0x22;
	fModule = htons(mod);
	fCh = htons(ch);
	fTime = htons(time);
    };
    ULong64_t GetVal() { return fVal; };
    UInt_t  * GetVal32() { return fVal32; };
    UChar_t * GetBuf() { return fBuf; };
    Int_t GetType() { return fHeader; };
    UInt_t GetTime() { return ntohs(fTime); };
    UInt_t GetCh() { return ntohs(fCh); };
    UInt_t GetModule() { return ntohs(fModule); };
};
#endif
