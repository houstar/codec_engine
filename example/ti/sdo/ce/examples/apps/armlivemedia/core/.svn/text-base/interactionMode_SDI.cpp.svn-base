#include "interactionMode_SDI.h"
#include "OMXComponentUtils.h"
#include "v4lDisplay.h"
#include "../utils/commonUtils.h"
#include "../kernel.h"

extern "C" Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);

InteractionMode_SDI::InteractionMode_SDI(Kernel* pKernel) :
		MixtureMode_SDI(pKernel) {
}

InteractionMode_SDI::~InteractionMode_SDI() {

}


bool InteractionMode_SDI::calcScalerWidthHeight() {
	bool bRet = false;
	if (m_pSetting->getDualScreenCrop()) {
		int nOutScaleWidth0 = m_outScaleWidth[0];
		int nOutScaleHeight0 = m_outScaleHeight[0];
		int nOutScaleWidth1 = m_outScaleWidth[1];
		int nOutScaleHeight1 = m_outScaleHeight[1];
		int nOutVGAWidth = m_outVGAWidth;
		int nOutVGAHeight = m_outVGAHeight;

		m_bDirectCopyChannel1 = false;
		m_bDirectCopyChannel2 = false;

		if ((m_pSetting->getDeviceSceneVideoInfo()->dwLeft != m_pSetting->getDeviceSceneVideoInfo()->dwRight)
				&& (m_pSetting->getDeviceSceneVideoInfo()->dwBottom != m_pSetting->getDeviceSceneVideoInfo()->dwTop)) {
			memcpy(&m_teacherSceneInfo, m_pSetting->getTeacherSceneVideoInfo(), sizeof(SceneVideoInfo));
			memcpy(&m_studentSceneInfo, m_pSetting->getStudentSceneVideoInfo(), sizeof(SceneVideoInfo));
			memcpy(&m_deviceSceneInfo, m_pSetting->getDeviceSceneVideoInfo(), sizeof(SceneVideoInfo));


			if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft == HD_WIDTH
					&& m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop == HD_HEIGHT) {
				m_bDirectCopyChannel1 = true;
			}

			if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft == HD_WIDTH
					&& m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop == HD_HEIGHT) {
				m_bDirectCopyChannel2 = true;
			}

			if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft != 0) {
				m_outScaleWidth[0] = m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft;

				if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
					m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
				} else {
					m_outScaleWidth[1] = m_outScaleWidth[0];
				}

				if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
					m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
				} else {
					m_outVGAWidth = m_outScaleWidth[0];
				}
			} else {
				if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
					m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
					m_outScaleWidth[0] = m_outScaleWidth[1];

					if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
						m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
					} else {
						m_outVGAWidth = m_outScaleWidth[0];
					}
				} else {
					if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
						m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
						m_outScaleWidth[0] = m_outVGAWidth;
						m_outScaleWidth[1] = m_outVGAWidth;
					} else {
						m_outVGAWidth = m_pSetting->getBaseRecordParams()->dwWidth;
						m_outScaleWidth[0] = m_outVGAWidth;
						m_outScaleWidth[1] = m_outVGAWidth;
					}
				}
			}

			if (m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop != 0) {
				m_outScaleHeight[0] = m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop;

				if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
					m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
				} else {
					m_outScaleHeight[1] = m_outScaleHeight[0];
				}

				if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
					m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
				} else {
					m_outVGAHeight = m_outScaleHeight[0];
				}
			} else {
				if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
					m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
					m_outScaleHeight[0] = m_outScaleHeight[1];

					if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
						m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
					} else {
						m_outVGAHeight = m_outScaleHeight[0];
					}
				} else {
					if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
						m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
						m_outScaleHeight[0] = m_outVGAHeight;
						m_outScaleHeight[1] = m_outVGAHeight;
					} else {
						m_outVGAHeight = m_pSetting->getBaseRecordParams()->dwHeight;
						m_outScaleHeight[0] = m_outVGAHeight;
						m_outScaleHeight[1] = m_outVGAHeight;
					}
				}
			}
			bRet = 	(nOutScaleWidth0 != m_outScaleWidth[0])
			   || (nOutScaleHeight0 != m_outScaleHeight[0])
			   || (nOutScaleWidth1 != m_outScaleWidth[1])
			   || (nOutScaleHeight1 != m_outScaleHeight[1])
			   || (nOutVGAWidth != m_outVGAWidth)
			   || (nOutVGAHeight != m_outVGAHeight);

		} else {
			if (m_teacherSceneInfo.dwLeft == m_pSetting->getTeacherSceneVideoInfo()->dwLeft
					&& m_teacherSceneInfo.dwRight == m_pSetting->getTeacherSceneVideoInfo()->dwRight
					&& m_teacherSceneInfo.dwBottom == m_pSetting->getTeacherSceneVideoInfo()->dwBottom
					&& m_teacherSceneInfo.dwTop == m_pSetting->getTeacherSceneVideoInfo()->dwTop
					&& m_studentSceneInfo.dwLeft == m_pSetting->getStudentSceneVideoInfo()->dwLeft
					&& m_studentSceneInfo.dwRight == m_pSetting->getStudentSceneVideoInfo()->dwRight
					&& m_studentSceneInfo.dwBottom == m_pSetting->getStudentSceneVideoInfo()->dwBottom
					&& m_studentSceneInfo.dwTop == m_pSetting->getStudentSceneVideoInfo()->dwTop
					&& m_deviceSceneInfo.dwLeft == m_pSetting->getDeviceSceneVideoInfo()->dwLeft
					&& m_deviceSceneInfo.dwRight == m_pSetting->getDeviceSceneVideoInfo()->dwRight
					&& m_deviceSceneInfo.dwBottom == m_pSetting->getDeviceSceneVideoInfo()->dwBottom
					&& m_deviceSceneInfo.dwTop == m_pSetting->getDeviceSceneVideoInfo()->dwTop) {
				bRet = false;
			} else {
				bRet = true;
			}
			memcpy(&m_teacherSceneInfo, m_pSetting->getTeacherSceneVideoInfo(), sizeof(SceneVideoInfo));
			memcpy(&m_studentSceneInfo, m_pSetting->getStudentSceneVideoInfo(), sizeof(SceneVideoInfo));
			memcpy(&m_deviceSceneInfo, m_pSetting->getDeviceSceneVideoInfo(), sizeof(SceneVideoInfo));

			if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft == HD_WIDTH
					&& m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop == HD_HEIGHT) {
				m_bDirectCopyChannel1 = true;
			}

			if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft == HD_WIDTH
					&& m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop == HD_HEIGHT) {
				m_bDirectCopyChannel2 = true;
			}

			m_outScaleWidth[0] = m_pSetting->getBaseRecordParams()->dwWidth;
			m_outScaleHeight[0] = m_pSetting->getBaseRecordParams()->dwHeight;

			m_outScaleWidth[1] = m_outScaleWidth[0];
			m_outScaleHeight[1] = m_outScaleHeight[0];

			if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
				m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
			} else {
				m_outVGAWidth = m_outScaleWidth[0];
			}

			if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
				m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
			} else {
				m_outVGAHeight = m_outScaleHeight[0];
			}

			m_bTeacherContainStud = false;
			m_bTeacherContainVGA = false;

			m_bStudContainTeacher = false;
			m_bStudContainVGA = false;

			m_bVGAContainTeacher = false;
			m_bVGAContainStud = false;
		}
	} else {
		int nOutScaleWidth0 = m_outScaleWidth[0];
		int nOutScaleHeight0 = m_outScaleHeight[0];
		int nOutScaleWidth1 = m_outScaleWidth[1];
		int nOutScaleHeight1 = m_outScaleHeight[1];
		int nOutVGAWidth = m_outVGAWidth;
		int nOutVGAHeight = m_outVGAHeight;
		memcpy(&m_teacherSceneInfo, m_pSetting->getTeacherSceneVideoInfo(), sizeof(SceneVideoInfo));
		memcpy(&m_studentSceneInfo, m_pSetting->getStudentSceneVideoInfo(), sizeof(SceneVideoInfo));
		memcpy(&m_deviceSceneInfo, m_pSetting->getDeviceSceneVideoInfo(), sizeof(SceneVideoInfo));


		if (m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft != 0) {
			m_outScaleWidth[0] = m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft;

			if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
				m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
			} else {
				m_outScaleWidth[1] = m_outScaleWidth[0];
			}

			if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
				m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
			} else {
				m_outVGAWidth = m_outScaleWidth[0];
			}
		} else {
			if (m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft != 0) {
				m_outScaleWidth[1] = m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft;
				m_outScaleWidth[0] = m_outScaleWidth[1];

				if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
					m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
				} else {
					m_outVGAWidth = m_outScaleWidth[0];
				}
			} else {
				if (m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft != 0) {
					m_outVGAWidth = m_deviceSceneInfo.dwRight - m_deviceSceneInfo.dwLeft;
					m_outScaleWidth[0] = m_outVGAWidth;
					m_outScaleWidth[1] = m_outVGAWidth;
				} else {
					m_outVGAWidth = m_pSetting->getBaseRecordParams()->dwWidth;
					m_outScaleWidth[0] = m_outVGAWidth;
					m_outScaleWidth[1] = m_outVGAWidth;
				}
			}
		}

		if (m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop != 0) {
			m_outScaleHeight[0] = m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop;

			if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
				m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
			} else {
				m_outScaleHeight[1] = m_outScaleHeight[0];
			}

			if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
				m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
			} else {
				m_outVGAHeight = m_outScaleHeight[0];
			}
		} else {
			if (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop != 0) {
				m_outScaleHeight[1] = m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop;
				m_outScaleHeight[0] = m_outScaleHeight[1];

				if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
					m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
				} else {
					m_outVGAHeight = m_outScaleHeight[0];
				}
			} else {
				if (m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop != 0) {
					m_outVGAHeight = m_deviceSceneInfo.dwBottom - m_deviceSceneInfo.dwTop;
					m_outScaleHeight[0] = m_outVGAHeight;
					m_outScaleHeight[1] = m_outVGAHeight;
				} else {
					m_outVGAHeight = m_pSetting->getBaseRecordParams()->dwHeight;
					m_outScaleHeight[0] = m_outVGAHeight;
					m_outScaleHeight[1] = m_outVGAHeight;
				}
			}
		}

		printf("before calc contain relationship\n");
		if (m_teacherSceneInfo.dwLeft != m_teacherSceneInfo.dwRight && m_teacherSceneInfo.dwTop != m_teacherSceneInfo.dwBottom) {
			m_bStudContainTeacher = m_studentSceneInfo.dwLeft <= m_teacherSceneInfo.dwLeft && m_studentSceneInfo.dwRight >= m_teacherSceneInfo.dwRight
					&& m_studentSceneInfo.dwTop <= m_teacherSceneInfo.dwTop && m_studentSceneInfo.dwBottom >= m_teacherSceneInfo.dwBottom;
			m_bVGAContainTeacher = m_deviceSceneInfo.dwLeft <= m_teacherSceneInfo.dwLeft && m_deviceSceneInfo.dwRight >= m_teacherSceneInfo.dwRight
					&& m_deviceSceneInfo.dwTop <= m_teacherSceneInfo.dwTop && m_deviceSceneInfo.dwBottom >= m_teacherSceneInfo.dwBottom;
		} else {
			m_bStudContainTeacher = false;
			m_bVGAContainTeacher = false;
		}

		if (m_studentSceneInfo.dwLeft != m_studentSceneInfo.dwRight && m_studentSceneInfo.dwTop != m_studentSceneInfo.dwBottom) {
			m_bTeacherContainStud = m_teacherSceneInfo.dwLeft <= m_studentSceneInfo.dwLeft && m_teacherSceneInfo.dwRight >= m_studentSceneInfo.dwRight
					&& m_teacherSceneInfo.dwTop <= m_studentSceneInfo.dwTop && m_teacherSceneInfo.dwBottom >= m_studentSceneInfo.dwBottom;
			m_bVGAContainStud = m_deviceSceneInfo.dwLeft <= m_studentSceneInfo.dwLeft && m_deviceSceneInfo.dwRight >= m_studentSceneInfo.dwRight
					&& m_deviceSceneInfo.dwTop <= m_studentSceneInfo.dwTop && m_deviceSceneInfo.dwBottom >= m_studentSceneInfo.dwBottom;
		} else {
			m_bTeacherContainStud = false;
			m_bVGAContainStud = false;
		}

		if (m_deviceSceneInfo.dwLeft != m_deviceSceneInfo.dwRight && m_deviceSceneInfo.dwTop != m_deviceSceneInfo.dwBottom) {
			m_bTeacherContainVGA = m_teacherSceneInfo.dwLeft <= m_deviceSceneInfo.dwLeft && m_teacherSceneInfo.dwRight >= m_deviceSceneInfo.dwRight
					&& m_teacherSceneInfo.dwTop <= m_deviceSceneInfo.dwTop && m_teacherSceneInfo.dwBottom >= m_deviceSceneInfo.dwBottom;
			m_bStudContainVGA = m_studentSceneInfo.dwLeft <= m_deviceSceneInfo.dwLeft && m_studentSceneInfo.dwRight >= m_deviceSceneInfo.dwRight
					&& m_studentSceneInfo.dwTop <= m_deviceSceneInfo.dwTop && m_studentSceneInfo.dwBottom >= m_deviceSceneInfo.dwBottom;
		} else {
			m_bTeacherContainVGA = false;
			m_bStudContainVGA = false;
		}

		if (m_bStudContainTeacher) {
			printf("stud contain teacher\n");
		}

		if (m_bVGAContainTeacher) {
			printf("vga contain teacher\n");
		}

		if (m_bTeacherContainStud) {
			printf("teacher contain stud\n");
		}
		if (m_bVGAContainStud) {
			printf("vga contain stud\n");
		}
		if (m_bTeacherContainVGA) {
			printf("teacher contain vga\n");
		}
		if (m_bStudContainVGA) {
			printf("stud contain vga\n");
		}
		bRet = 	(nOutScaleWidth0 != m_outScaleWidth[0])
		   || (nOutScaleHeight0 != m_outScaleHeight[0])
		   || (nOutScaleWidth1 != m_outScaleWidth[1])
		   || (nOutScaleHeight1 != m_outScaleHeight[1])
		   || (nOutVGAWidth != m_outVGAWidth)
		   || (nOutVGAHeight != m_outVGAHeight);
	}

#ifdef BUFFER_ALLOCATE_IN_CLIENT
	if (m_bStudContainTeacher || m_bVGAContainTeacher) {
		if (NULL == m_pContainTeacherImage) {
			m_nContainTeacherSize = (pTeacherInfo->dwRight - pTeacherInfo->dwLeft) * (pTeacherInfo->dwBottom - pTeacherInfo->dwTop) * 2;
			m_pContainTeacherImage = (unsigned char*)Memory_alloc(m_heap, m_nContainTeacherSize, 128, NULL);
			m_containTeacherImagePhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_pContainTeacherImage);
			Memory_set(m_pContainTeacherImage, 0, m_nContainTeacherSize);
			SempUtils::semp_init(&m_containTeacherSem, 1);
		}
	}

	if (m_bTeacherContainStud || m_bVGAContainStud) {
		if (NULL == m_pContainStudImage) {
			m_nContainStudSize = (pStudInfo->dwRight - pStudInfo->dwLeft) * (pStudInfo->dwBottom - pStudInfo->dwTop) * 2;
			m_pContainStudImage = (unsigned char*)Memory_alloc(m_heap, m_nContainStudSize, 128, NULL);
			m_containStudImagePhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_pContainStudImage);
			Memory_set(m_pContainStudImage, 0, m_nContainStudSize);


			SempUtils::semp_init(&m_containStudSem, 1);
		}
	}

	if (m_bTeacherContainVGA || m_bStudContainVGA) {
		if (NULL == m_pContainVGAImage) {
			m_nContainVGASize = (pDeviceInfo->dwRight - pDeviceInfo->dwLeft) * (pDeviceInfo->dwBottom - pDeviceInfo->dwTop) * 2;
			m_pContainVGAImage = (unsigned char*)Memory_alloc(m_heap, m_nContainVGASize, 128, NULL);
			m_containVGAImagePhyAddr = (OMX_PTR)DomxCore_mapUsrVirtualAddr2phy((uint32_t)m_pContainVGAImage);
			Memory_set(m_pContainVGAImage, 0, m_nContainVGASize);
			SempUtils::semp_init(&m_containVGASem, 1);
		}
	}
#endif
	printf("after calc contain relationship\n");

	return bRet;
}


void InteractionMode_SDI::onSDIScaleChannel1(ComponentWrapper* aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	OutportParams *outPortParamsPtr = m_sdiScaleComponent->outPortParams;
	while (1) {
		read(outPortParamsPtr->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s write thread\n", aComponent->name);
		    pthread_exit(NULL);
		    break;
		}
		m_exchangeMutex.lock();
		if (!m_bHeadState && !m_bTailState) {
			if (m_teacherSceneInfo.dwLeft != m_teacherSceneInfo.dwRight
					&& m_teacherSceneInfo.dwTop != m_teacherSceneInfo.dwBottom && !m_ResumeTimerValid) {
				//printf("before DomxCore_mapUsrVirtualAddr2phy 6\n");
				OMX_PTR pOutAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy( (uint32_t) pBufferOut->pBuffer);
				//printf("after DomxCore_mapUsrVirtualAddr2phy 6\n");

				if (m_bStudContainTeacher || m_bVGAContainTeacher) {
					if (pBufferOut->nFilledLen == m_outScaleWidth[0] * m_outScaleHeight[0] * 2) {
						SempUtils::semp_pend(&m_containTeacherSem);
						m_pKernel->getEdmaInstance()->copy(m_containTeacherImagePhyAddr, pOutAddr,
								m_outScaleWidth[0] * 2, m_outScaleWidth[0] * 2,
								pBufferOut->nFilledLen, 1);
						SempUtils::semp_post(&m_containTeacherSem);
					}
				} else {
					if (pBufferOut->nFilledLen == m_outScaleWidth[0] * m_outScaleHeight[0] * 2 && !m_bDirectCopyChannel1) {
						if ((m_deviceSceneInfo.dwLeft != m_deviceSceneInfo.dwRight)
								&& (m_deviceSceneInfo.dwBottom != m_deviceSceneInfo.dwTop)) {
							m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
											+ m_teacherSceneInfo.dwTop * m_outWidth * 2
											+ m_teacherSceneInfo.dwLeft * 2, pOutAddr,
									m_outScaleWidth[0] * 2, m_outWidth * 2,
									pBufferOut->nFilledLen, 1);
						} else {
							m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr + m_teacherSceneInfo.dwTop * m_outWidth * 2 + m_teacherSceneInfo.dwLeft * 2,
									(char*)pOutAddr + (m_outScaleHeight[0] + m_teacherSceneInfo.dwTop - m_teacherSceneInfo.dwBottom)/2 * m_outScaleWidth[0] * 2 + (m_outScaleWidth[0] + m_teacherSceneInfo.dwLeft - m_teacherSceneInfo.dwRight)/2 * 2,
									(m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft) * 2, m_outScaleWidth[0] * 2,
									m_outWidth * 2, m_outWidth * 2,
									(m_teacherSceneInfo.dwRight - m_teacherSceneInfo.dwLeft) * (m_teacherSceneInfo.dwBottom - m_teacherSceneInfo.dwTop) * 2, 1);
						}
					}

					if (m_bTeacherContainStud) {
						SempUtils::semp_pend(&m_containStudSem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr + m_studentSceneInfo.dwTop * m_outWidth * 2
										+ m_studentSceneInfo.dwLeft * 2,
								m_containStudImagePhyAddr, m_outScaleWidth[1] * 2, m_outWidth * 2,
								m_nContainStudSize, 1);
						SempUtils::semp_post(&m_containStudSem);
					} else if (m_bTeacherContainVGA) {
						SempUtils::semp_pend(&m_containVGASem);
						m_pKernel->getEdmaInstance()->copy( (char*) m_exchangeImagePhyAddr + m_deviceSceneInfo.dwTop * m_outWidth * 2
								+ m_deviceSceneInfo.dwLeft * 2,
								m_containVGAImagePhyAddr, m_outVGAWidth * 2,
								m_outWidth * 2, m_nContainVGASize, 1);
						SempUtils::semp_post(&m_containVGASem);
					}
				}
			}
		}
		m_exchangeMutex.unlock();

	    err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);
		/*struct timespec tp;
		if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
			perror("2");
		}
		printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
		if (OMX_ErrorNone != err) {
			write(outPortParamsPtr->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

void InteractionMode_SDI::onSDIScaleChannel2(ComponentWrapper* aComponent) {
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
	OutportParams *outPortParamsPtr;
	while (1) {
		outPortParamsPtr = m_sdiScaleComponent->outPortParams + 1;
		read(outPortParamsPtr->opBufPipe[0], &pBufferOut, sizeof(pBufferOut));
		if (m_bExit)  {
			SempUtils::semp_post(aComponent->eos_sem);
			printf("exit %s write thread\n", aComponent->name);
		    pthread_exit(NULL);
		    break;
		}
		m_exchangeMutex.lock();
		if (!m_bHeadState && ! m_bTailState) {
			if (m_studentSceneInfo.dwLeft != m_studentSceneInfo.dwRight
					&& m_studentSceneInfo.dwTop != m_studentSceneInfo.dwBottom && !m_ResumeTimerValid) {
				//printf("before DomxCore_mapUsrVirtualAddr2phy 8\n");
				OMX_PTR pOutAddr = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy((uint32_t) pBufferOut->pBuffer);
				//printf("after DomxCore_mapUsrVirtualAddr2phy 8\n");

				if (m_bTeacherContainStud || m_bVGAContainStud) {
					if (pBufferOut->nFilledLen == m_nContainStudSize) {
						SempUtils::semp_pend(&m_containStudSem);
						m_pKernel->getEdmaInstance()->copy(m_containStudImagePhyAddr, pOutAddr,
								m_outScaleWidth[1] * 2, m_outScaleWidth[1] * 2,
								pBufferOut->nFilledLen, 1);
						SempUtils::semp_post(&m_containStudSem);
					}
				} else {
					if (pBufferOut->nFilledLen == m_outScaleWidth[1] * m_outScaleHeight[1] * 2 && !m_bDirectCopyChannel2) {
						if ((m_deviceSceneInfo.dwLeft != m_deviceSceneInfo.dwRight)
							&& (m_deviceSceneInfo.dwBottom != m_deviceSceneInfo.dwTop)) {
							m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
											+ m_studentSceneInfo.dwTop * m_outWidth * 2
											+ m_studentSceneInfo.dwLeft * 2, pOutAddr,
									m_outScaleWidth[1] * 2, m_outWidth * 2,
									pBufferOut->nFilledLen, 1);
						} else {
							m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr + m_studentSceneInfo.dwTop * m_outWidth * 2 + m_studentSceneInfo.dwLeft * 2,
									(char*)pOutAddr + (m_outScaleHeight[1] + m_studentSceneInfo.dwTop - m_studentSceneInfo.dwBottom) /2 * m_outScaleWidth[1] * 2 + (m_outScaleWidth[1] + m_studentSceneInfo.dwLeft - m_studentSceneInfo.dwRight)/2 * 2,
									(m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft) * 2, m_outScaleWidth[1] * 2,
									m_outWidth * 2, m_outWidth * 2,
									(m_studentSceneInfo.dwRight - m_studentSceneInfo.dwLeft) * (m_studentSceneInfo.dwBottom - m_studentSceneInfo.dwTop) * 2, 1);
						}
					}
					if (m_bStudContainTeacher) {
						SempUtils::semp_pend(&m_containTeacherSem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_teacherSceneInfo.dwTop * m_outWidth * 2
										+ m_teacherSceneInfo.dwLeft * 2,
								m_containTeacherImagePhyAddr,
								m_outScaleWidth[0] * 2, m_outWidth * 2,
								m_nContainTeacherSize, 1);
						SempUtils::semp_post(&m_containTeacherSem);
					} else if (m_bStudContainVGA) {
						SempUtils::semp_pend(&m_containVGASem);
						m_pKernel->getEdmaInstance()->copy((char*) m_exchangeImagePhyAddr
										+ m_deviceSceneInfo.dwTop * m_outWidth * 2
										+ m_deviceSceneInfo.dwLeft * 2,
								m_containVGAImagePhyAddr, m_outVGAWidth * 2,
								m_outWidth * 2, m_nContainVGASize, 1);
						SempUtils::semp_post(&m_containVGASem);
					}

				}
			}
		}
		m_exchangeMutex.unlock();
	    err = OMX_FillThisBuffer (aComponent->handle, pBufferOut);
		/*struct timespec tp;
		if(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp)){
			perror("2");
		}
		printf("componenent name %s >>%ds.%dns\n", aComponent->name, tp.tv_sec,tp.tv_nsec);*/
		if (OMX_ErrorNone != err) {
			write(outPortParamsPtr->opBufPipe[1],	&pBufferOut, sizeof(pBufferOut));
			printf(" waiting for action from IL Client %x\n", err);
			SempUtils::semp_post(aComponent->eos_sem);
			pthread_exit(NULL);
			break;
		}
	}
}

