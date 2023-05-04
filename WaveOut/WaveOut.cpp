#define VERSION 2

#pragma comment(lib, "winmm.lib")
#include<iostream>
#include<Windows.h>
#include<vector>
#include<math.h>
#include"AudioFile.h"
using namespace std;

unsigned int m_nBlockFree;

double NoiseMaker(double freq, double dTime) {
	return 0.5 * sin(freq * 2.0 * 3.1415 * dTime);
	//시간에 따른 sin 함수값을 반환한다.
}

double clip(double dSample, double dMax)
{
	if (dSample >= 0.0)
		return fmin(dSample, dMax);
	else
		return fmax(dSample, -dMax);

	//double 진폭값을 short형의 범위에 속하는 값으로 변하게 하기위한 함수
	//주파수가 중요하기 때문에 진폭의 크기보다는 진폭의 속도변화 비율이 더 중요하다
	//그래서 short에 저장되는 최대정수에 double 진폭값을 곱해서 정수로 바꾼다
}

void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg != WOM_DONE) return;
	m_nBlockFree++;
	//한 블록(버퍼) 출력이 끝났다는걸 나타낸다
	//그나저나 이렇게 이중으로 하는 이유가 뭐지..?
}

static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	//waveoutwrite에서 전달된 버퍼가 모두 비워지면 호출되는 듯
	waveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
}

int main() {
#if(VERSION==1)
	double freq; //출력할 주파수

	wcout << "출력할 주파수를 입력하세요: ";
	wcin >> freq; //440이나 600 등을 입력하면 그 주파수에 해당하는 소리를 출력한다
#elif(VERSION==2)
	AudioFile<double> audio;
	audio.load("SMI.wav");
#endif

	HWAVEOUT m_hwDevice;
	short* buffer; //wavhdr.lpdata에 연결할 short 버퍼(2바이트)
	WAVEHDR* m_pWaveHeader;	//소리 데이터가 담겨있다

	unsigned int m_nBlockSamples = 512; //버퍼 하나당 샘플수
	unsigned int m_nSampleRate = 44100; //1초에 재생되는 샘플수
	unsigned int m_nBlockCount = 8; //버퍼의 개수

	m_nBlockFree = m_nBlockCount; //현재 남아도는 버퍼를 버퍼의 전체 개수로 설정한다

	WAVEFORMATEX waveFormat; //파형 오디오 데이터의 포맷을 결정한다
	waveFormat.wFormatTag = WAVE_FORMAT_PCM; //파형 오디오의 포맷 타입(압축 알고리즘용)
	waveFormat.nSamplesPerSec = m_nSampleRate; //샘플레이트
	waveFormat.wBitsPerSample = sizeof(short) * 8; //소리 샘플이 short형으로 저장되었다는 걸 데이터 크기를 통해 알려준다 //이 값은 무조건 8bit 아니면 16bit이다 
	//2^16 개의 숫자로 파형을 나타내겠다는 소리와 같다 //16만 보면 뭔가 부족할 것 같았는데 차고도 넘친다
	waveFormat.nChannels = 1; //소리가 재생되는 기기의 개수, 즉 채널개수이다
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; //샘플의 크기(바이트 단위) * 채널수
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; //1초에 필요한 바이트수
	waveFormat.cbSize = 0;

	//WaveOutOpen으로 기기를 연다
	if (waveOutOpen(&m_hwDevice, NULL, &waveFormat, (DWORD_PTR)waveOutProcWrap, (DWORD_PTR)0, CALLBACK_FUNCTION) != S_OK)
		return 0;
	/*
	[고쳐야할점]
	여기서는 waveoutopen의 두번째 인자인 오디오 출력에 사용할 장치의 식별자를 0으로 했다
	이는 재생가능한 오디오 출력장치를 자동으로 찾아주는 매크로이다
	실제로는 오디오 디바이스의 식별자를 찾는 과정을 거쳐야 하므로 이 부분은 수정이 필요하다
	*/

	m_pWaveHeader = new WAVEHDR[m_nBlockCount]; //버퍼 개수만큼 wavehdr 메모리 할당
	ZeroMemory(m_pWaveHeader, sizeof(WAVEHDR) * m_nBlockCount); //메모리를 모두 비운다

	buffer = new short[m_nBlockCount * m_nBlockSamples]; //총 버퍼의 크기인 버퍼 개수*버퍼 길이 만큼 메모리를 할당한다
	ZeroMemory(buffer, sizeof(short) * m_nBlockCount * m_nBlockSamples); //마찬가지로 메모리를 모두 비운다

	for (unsigned int n = 0; n < m_nBlockCount; n++) {
		m_pWaveHeader[n].lpData = (LPSTR)(buffer + (n * m_nBlockSamples)); //lpdata와 버퍼를 연결한다
		m_pWaveHeader[n].dwBufferLength = m_nBlockSamples * sizeof(short); //lpdata에 연결된 버퍼의 크기를 알려준다
	}

	short sdmaxsample = (pow(2, (sizeof(short) * 8) - 1) - 1); //short로 표현할 수 있는 최대 정수를 구한다(이진법을 십진법으로)
	double dMaxSample = (double)sdmaxsample; //그 값을 double로 형변환
	double m_dGlobalTime = 0.0; //출력시간을 저장
	double dTimeStep = 1.0 / (double)m_nSampleRate; //한 샘플당 걸리는 시간

	int m_nBlockCurrent = 0; //현재 출력중인 버퍼
	short nNewSample = 0; //샘플 단위버퍼

	unsigned int m_currentSample = 0;

#if(VERSION==1)
	double playTime = 10.0
#elif(VERSION==2)
	double playTime = audio.getNumSamplesPerChannel() / audio.getSampleRate()  - 1.0;
#endif

	while (m_dGlobalTime < playTime) {
		if (m_nBlockFree != 0) {
			//만약 놀고있는 버퍼가 있다면
			m_nBlockFree--; //놀고있는 버퍼 하나를 줄임

			int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples; //처리하는 버퍼의 시작점 0, 512, 1024 ...

			//wavehdr 버퍼를 준비시킨다
			if (m_pWaveHeader[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));

			for (unsigned int n = 0; n < m_nBlockSamples; n++)
			{
#if(VERSION==1)
				nNewSample = (short)(clip(NoiseMaker(freq, m_dGlobalTime), 1.0) * dMaxSample);
				//현재 시간에 해당하는 파동값을 가져옴
#elif(VERSION==2)
				nNewSample = (short)(clip(audio.samples[0][floor(m_dGlobalTime * m_nSampleRate)], 1.0) * dMaxSample);
#endif
				buffer[nCurrentBlock + n] = nNewSample;
				//버퍼의 샘플개수만큼 읽어들임

				m_dGlobalTime = m_dGlobalTime + dTimeStep;
				//측정시간 증가시킴
			}

			waveOutPrepareHeader(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
			waveOutWrite(m_hwDevice, &m_pWaveHeader[m_nBlockCurrent], sizeof(WAVEHDR));
			//소리 출력

			m_currentSample += m_nBlockSamples; //현재 출력중인 샘플 위치 //이 코드에서는 쓰이지 않는다
			wcout << m_currentSample << endl;

			m_nBlockCurrent++; //다음버퍼로 이동
			m_nBlockCurrent %= m_nBlockCount; //버퍼의 개수 안에서 현재 버퍼를 계산해주는 나머지 연산
		}
	}

	MMRESULT errorcode = waveOutPause((HWAVEOUT)m_hwDevice);
	if (errorcode == MMSYSERR_NOERROR) {
		waveOutReset((HWAVEOUT)m_hwDevice);

		for (int i = 0; i < m_nBlockCount; i++) {
			waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeader[i], sizeof(WAVEHDR));
		}
	}

	delete[] m_pWaveHeader;
	delete[] buffer;
}
