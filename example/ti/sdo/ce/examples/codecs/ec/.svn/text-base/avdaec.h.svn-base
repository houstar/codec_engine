
#ifndef       ___AVD_AUDIO_AEC_INTERFACE_H___
#define       ___AVD_AUDIO_AEC_INTERFACE_H___


#ifdef __cplusplus
extern "C" {
#endif

	
	typedef struct
	{
		int   sagc_param;      /*  软件自动增益控制（SAGC）处理参数 0->关闭SAGC,1->打开SAGC */
		int   hagc_param;      /*  硬件自动增益控制（HAGC）处理参数 0->关闭HAGC,1->打开HAGC */
		int	 hagc_max_vol;    /*  硬件AGC参数,声卡可调节最大音量 */
		int	 hagc_min_vol;    /*  硬件AGC参数,声卡可调节最小音量 */
		int	 hagc_in_vol;     /*  硬件AGC输入参数,当前硬件音量输入, 从声卡获取 */
		int	 hagc_out_vol;    /*  硬件AGC输出参数, 用于调节声卡MIC音量 */
	}T_avdAGCParam;
	
	
	typedef struct
	{	
		int   sample_rate;   /* 编解码器采样率:8k,16k，注意声卡采样率固定为48Khz */
		int   aec_param;     /*声学回音消除（AEC）处理参数 0->关闭AEC,1->打开AEC */
		int   ns_param;      /* 噪声消除（NS）处理参数 0->关闭NS,1->13db, 2->18db, 3->25db */
		int   buffercount;
		T_avdAGCParam   avdAgcParam;     /* 自动增益控制（AGC）处理参数, 详见前面定义 */		   
	}T_avdAECParam;
	
	/* AEC初始化函数 */
	extern int avd_AEC_Init(T_avdAECParam *pParam);	
	/* AEC主函数  */
	extern int avd_AEC_Proc(short *Sqe_near_ptr, short *Sqe_far_ptr, short *Sqe_out_ptr,int InVol);
	/* 退出AEC处理函数  */
	extern int avd_AEC_UnInit();
	
#ifdef __cplusplus
}
#endif

#endif//___AVD_AUDIO_AEC_INTERFACE_H___




