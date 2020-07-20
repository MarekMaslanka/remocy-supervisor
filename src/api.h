#ifndef __API_H_
#define __API_H_

#include <stdint.h>

#define IS_IMPORTANT_MSG(msg) (msg->seq & (1 << 63))
#define SET_IMPORTANT_MSG(msg) (msg->seq | (1 << 63))
#define CLEAR_IMPORTANT_MSG(msg) (msg->seq & ~(1 << 63))

namespace Remocy
{
	constexpr int MAX_RESOLUTIONS_WIDTH = 1920 * 2;
	constexpr int MAX_RESOLUTIONS_HEIGHT = 1080 * 2;
	constexpr int PACKET_SIZE = 8 * 1024;

	enum CompressionType
	{
		H264, RLE, RLE1, RLE4, RLE8, TRLE
	};

	// Client
	struct MouseEvent
	{
		uint32_t x, y;
		int scroll;
		uint32_t btn;
		int state;
		uint32_t modifier;
		int relative;
	};

	struct KeyboardEvent
	{
		int key;
		int modifier;
		uint32_t state;
		int unicode;
		int utf;
		int scanCode;
		int vkKey;
	};

	struct ServerSettings
	{
		uint32_t screenWidth;
		uint32_t screenHeight;
		uint32_t audioQuality;
	};

	struct ConnectionSettings
	{
		uint32_t fps;
		uint32_t throughput;
		uint32_t quality;
		uint32_t ackMode;
		uint32_t gameMode;
		CompressionType compression;
	};

	struct ClientSettings
	{
		uint32_t screenWidth;
		uint32_t screenHeight;
	};

	struct Screens
	{
		uint32_t count;
		uint32_t primaryIndex;
	};

	struct CursorRequest
	{
		uint32_t id;
		uint32_t chunk;
	};

	struct FileRequest
	{
		uint64_t offset;
	//    uint8_t path[];
	};

	struct FrameChunksRequest
	{
		uint32_t timestamp;
		uint32_t count;
		uint32_t ssrc;
	};

	enum MsgType
	{
		MsgMouseEvent = 1,
		MsgKeyboardEvent,
		MsgClipboardReq,
		MsgClipboardData,
		MsgRtpReq,
		MsgClientHello,
		MsgClientHandshakeFinish,
		MsgServerSettings,
		MsgConnectionSettings,
		MsgCursorReq,
		MsgScreensGeometry,
		MsgFileListReq,
		MsgFileReq,
		MsgClientSettings,
		MsgPing
	};

	struct ClientMsg
	{
		MsgType type;
		uint64_t seq;
		uint32_t dataSize;
		uint32_t hash;
		union
		{
			MouseEvent mouseEvent;
			KeyboardEvent keyboardEvent;
			ServerSettings serverSettings;
			ConnectionSettings connectionSettings;
			CursorRequest cursorRequest;
			FrameChunksRequest frameChunksRequest;
			Screens screens;
			int ping;
		} msg;
		uint8_t data[];
	};

	// Server
	enum ServerMsgType
	{
		MsgEventAck = 1,
		MsgServerClipboardData,
		MsgServerFileInfo,
		MsgServerFileData,
		MsgCursor,
		MsgCurrentServerSettings,
		MsgCurrentConnectionSettings,
		MsgFrameSync,
		MsgServerStats,
		MsgFrameAckReq,
		MsgFrameChunks,
	};

	struct RtpHeader
	{
		uint16_t flags;
		uint16_t seq;
		uint32_t timestamp;
		uint32_t ssrc;
		uint8_t data[];
	}
#if !defined(_WIN32)
	__attribute__((packed))
#endif
	;

	struct ServerAck
	{
		uint32_t seq;
	};

	struct CursorData
	{
		uint16_t id;
		uint32_t offset;
		uint16_t stride;
		uint16_t height;
		uint16_t xhot;
		uint16_t yhot;
	};

	struct FileInfo
	{
		uint64_t size;
		uint8_t type;
		// uint8_t path[];
	};

	struct FileData
	{
		uint32_t offset;
		uint32_t hash;
		// uint8_t pathAndData[];
	};

	struct FrameSyncInfo
	{
		uint16_t seq;
		uint32_t timestamp;
		uint16_t count;
		uint32_t len;
	};

	struct FrameAckReq
	{
		uint16_t seq;
		uint16_t count;
		uint32_t timestamp;
	};

	struct FrameRect
	{
		uint32_t x, y, width, height;
	};

	struct RleFrame
	{
		FrameRect rect;
		uint32_t qsize;
	};

	struct FrameChunk
	{
		RleFrame frame;
		uint32_t size;
		uint8_t data[];
	};

	struct ServerStats
	{
		struct System
		{
			int cpuUsage;
			int cpuClock;
			int ramTotal;
			int ramUsed;
		} system;
		struct Timings
		{
			int grabScreen;
			int findChanges;
			int convertYUV;
			int encode;
			int send;
			int total;
		} timings;
		struct Encoder
		{
			uint32_t singles;
		} encoder;
		uint32_t time;
	};

	union ServerMsg
	{
		struct RtpHeader rtp;
		struct
		{
			uint16_t dummyFF;
			uint64_t seq;
			int dataSize;
			ServerMsgType type;
			union
			{
				ServerAck ack;
				CursorData cursor;
				ServerSettings serverSettings;
				ConnectionSettings connectionSettings;
				FileInfo fileInfo;
				FileData fileData;
				ServerStats stats;
				FrameSyncInfo frameSyncInfo;
				FrameAckReq frameAck;
				int frameChunks;
			};
			uint8_t data[];
		} msg;
	}
#if !defined(_WIN32)
	__attribute__((packed))
#endif
	;

}

#endif // __API_H_

using namespace Remocy;
