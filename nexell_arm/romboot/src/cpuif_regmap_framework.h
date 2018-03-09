//=====================================================================================
// <xsl:value-of select="/cpuif/prefix"/>cpuif_regmap
//=====================================================================================
// usage example
//-------------------------------------------------------------------------------------
// nx_cpuif_reg_write( g_<xsl:value-of select="/cpuif/prefix"/>regmap[0]->DIRTYFLAT_TOP, 1 , CPUIF_END );
//
// nx_cpuif_reg_write( g_<xsl:value-of select="/cpuif/prefix"/>regmap[0].DIRTYFLAT_TOP, 1,
//				   	   g_<xsl:value-of select="/cpuif/prefix"/>regmap[0].FIELDENB, 1     , CPUIF_END  );
//
//
// int regval, regval2;
// nx_cpuif_reg_read ( g_<xsl:value-of select="/cpuif/prefix"/>regmap[0].DIRTYFLAG_TOP, &amp;regval , CPUIF_END  );
// nx_cpuif_reg_read ( g_<xsl:value-of select="/cpuif/prefix"/>regmap[0].DIRTYFLAG_TOP, &amp;regval,
//				   	   g_<xsl:value-of select="/cpuif/prefix"/>regmap[0].FIELDENB,      &amp;regval2, CPUIF_END );
//
//
// g_<xsl:value-of select="/cpuif/prefix"/>regmap[0] is a global struct (using template. for max moduleindex assert)
// 
// each REG's value is also struct (addr, startbit, bitwidth) 
// 
//=====================================================================================


#ifdef __NX_CPUIF_REG_FUNC_HEADER__
#else
#define __NX_CPUIF_REG_FUNC_HEADER__

// ARM compiler 
#ifdef __SOC__
#ifdef CPUIF_REGMAP_DEBUG
#define nx_cpuif_debug_print(expr...) NX_CONSOLE_Printf(expr)
#else
#define nx_cpuif_debug_print(expr...)  //printk(expr)
#endif
#else
#ifdef __arm
#define nx_cpuif_debug_print(expr...) printk(expr) 
#else
#define nx_cpuif_debug_print(expr...)  //printf(expr)
#endif
#endif

typedef struct __nx_cpuif_sym_struct__ {
	unsigned int *baseaddr;
	unsigned int offset;
	unsigned int startbit;
	unsigned int bitwidth;
} _nx_cpuif_sym_ ;

#define CPUIF_END (_nx_cpuif_sym_){0,0,0,0}
#define nx_cpuif_regmap_MAX_LIST 128


#ifdef	__cplusplus
extern "C"
{
#endif

#ifdef __SOC__
void NX_CONSOLE_Printf(const char *FormatString, ...);
#endif

void nx_cpuif_reg_write_one_notread(_nx_cpuif_sym_ symbol, unsigned int  regval);
void nx_cpuif_reg_write_one(_nx_cpuif_sym_ symbol, unsigned int   regval);
unsigned int  nx_cpuif_reg_read_one(_nx_cpuif_sym_ symbol, unsigned int * regval);

void nx_cpuif_reg_write(_nx_cpuif_sym_ symbol, unsigned int   regval, ...);
unsigned int  nx_cpuif_reg_read(_nx_cpuif_sym_ symbol, unsigned int * regval, ...);
#ifdef	__cplusplus
}
#endif

//============================
// C++의 Template를 사용할 경우, module index를 검사할 수 있다.
// C 일 경우에는, module index가 잘못들어왔는지는 검사할 수 없다. 
// 따라서 이런 검사는 Prototype 함수 내에서 해주어야 한다. 
//============================
// 김책임님에게 협조받은 코드 
// 각 모듈마다 따로 만들어 주어서, 에러시 어떤 regmap 접근에서 에러가 났었는지를 알려주도록 한다. 
#ifdef	__cplusplus
template<class T, unsigned int totalcount>
struct g_nx_cpuif_regmap_array
{
	// regmap struct usage
	T instance[totalcount];

	T& operator[] (unsigned int index) {
		if ((index>=totalcount) || (index>=1024)) {
			nx_cpuif_debug_print("\n*E, index must smaller than totalcount and 1024 [cpuif_regmap]" );
			nx_cpuif_debug_print("\n*E, index = %1d, maxindex = %1d, totalcount = %1d", index, totalcount - 1, totalcount );
			nx_cpuif_debug_print("\n*E, index fault occurs (infinite loop)");
			nx_cpuif_debug_print("\n*E, stop");
			nx_cpuif_debug_print("\n*E, stop");
			nx_cpuif_debug_print("\n*E, stop");
			while (1);
		}
		return instance[index];
	}
};
#endif // #ifdef	__cplusplus

#endif // #ifdef __NX_CPUIF_REG_FUNC_HEADER__
