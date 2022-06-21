#include "DotMaliciousLeaky.h"
#ifdef ENABLE_SOFTSPOKEN_OT

#include "TwoOneMalicious.h"

namespace osuCrypto
{
	namespace SoftSpokenOT
	{

		template<typename Hasher1>
		void DotMaliciousLeakySender::sendImpl(
			span<std::array<block, 2>> messages, PRNG& prng, Channel& chl, Hasher1& hasher)
		{
			if (!hasBaseOts())
				genBaseOts(prng, chl);

			size_t nChunks = divCeil(messages.size() + 64, 128);
			size_t messagesFullChunks = messages.size() / 128;

			block* scratch = (block*)messages.data();
			AlignedUnVector<block> scratchBacking;
			if (wSize() > 2 * 128)
			{
				scratchBacking.resize(messagesFullChunks * chunkSize() + paddingSize());
				scratch = scratchBacking.data();
			}

			ChunkerBase::runBatch(chl, span<block>(scratch, messagesFullChunks * chunkSize()));

			// Extra blocks
			size_t numExtra = nChunks - messagesFullChunks; // Always 1 or 2
			ChunkerBase::runBatch(chl, mExtraW.subspan(0, numExtra * chunkSize()));

			mVole->sendChallenge(prng, chl);
			hasher.send(prng, chl);

			hasher.runBatch(chl, messages.subspan(0, messagesFullChunks * 128), this, scratch);
			hasher.runBatch(chl, messages.subspan(messagesFullChunks * 128), this, mExtraW.data());

			// Hash the last extra block if there was one with no used mMessages in it at all.
			if (numExtra == 2 || messages.size() % 128 == 0)
				mVole->hash(mExtraW.subspan(chunkSize() * (numExtra - 1), wPadded()));

			mVole->checkResponse(chl);
		}

		void DotMaliciousLeakySender::processChunk(size_t nChunk, size_t numUsed, span<block> messages)
		{
			size_t blockIdx = mFieldBitsThenBlockIdx++;
			mVole->generateChosen(blockIdx, useAES(mVole->mVole.mNumVoles), messages.subspan(0, wPadded()));
		}

		void DotMaliciousLeakySender::Hasher::processChunk(
			size_t nChunk, size_t numUsed,
			span<std::array<block, 2>> messages,
			DotMaliciousLeakySender* parent, block* inputW)
		{
			inputW += nChunk * parent->chunkSize();
			parent->mVole->hash(span<const block>(inputW, parent->wPadded()));

			transpose128(inputW);
			parent->xorMessages(numUsed, (block*)messages.data(), inputW);
		}

		template<typename Hasher1>
		void DotMaliciousLeakyReceiver::receiveImpl(
			const BitVector& choices, span<block> messages, PRNG& prng, Channel& chl, Hasher1& hasher)
		{
			if (!hasBaseOts())
				genBaseOts(prng, chl);

			size_t nChunks = divCeil(messages.size() + 64, 128);
			size_t messagesFullChunks = messages.size() / 128;

			block* scratch = (block*)messages.data();
			AlignedUnVector<block> scratchBacking;
			if (vSize() > 128)
			{
				scratchBacking.resize(messagesFullChunks * chunkSize() + paddingSize());
				scratch = scratchBacking.data();
			}

			ChunkerBase::runBatch<block>(
				chl, span<block>(scratch, messagesFullChunks * chunkSize()),
				span<block>(choices.blocks(), messagesFullChunks));

			// Extra blocks
			size_t numExtra = nChunks - messagesFullChunks; // Always 1 or 2
			u64 extraChoicesU64[4] = { 0 };
			u64 sacrificialChoices = prng.get<u64>();
			int bit = messages.size() % 128;
			if (bit)
				memcpy(extraChoicesU64, choices.blocks() + messagesFullChunks, sizeof(block));

			int bit64 = bit % 64;
			int word = bit / 64;
			u64 mask = ((u64)1 << bit64) - 1;
			extraChoicesU64[word] &= mask;
			extraChoicesU64[word] |= sacrificialChoices << bit64;
			// Shift twice so that it becomes zero if bit64 = 0 (shift by 64 is undefined).
			extraChoicesU64[word + 1] = sacrificialChoices >> (63 - bit64) >> 1;

			block extraChoices[2] = { toBlock(0,0), toBlock(0,0) };
			memcpy(extraChoices, extraChoicesU64, 2 * sizeof(block));

			ChunkerBase::runBatch<block>(
				chl, mExtraV.subspan(0, numExtra * chunkSize()),
				span<block>(extraChoices, numExtra));

			mVole->recvChallenge(chl);
			hasher.recv(chl);

			hasher.template runBatch<block>(
				chl, messages.subspan(0, messagesFullChunks * 128),
				span<block>(choices.blocks(), messagesFullChunks), this, scratch);
			hasher.template runBatch<block>(
				chl, messages.subspan(messagesFullChunks * 128),
				span<block>(extraChoices, messages.size() % 128 != 0), this, mExtraV.data());

			// Hash the last extra block if there was one with no used mMessages in it at all.
			if (numExtra == 2 || messages.size() % 128 == 0)
				mVole->hash(
					span<block>(&extraChoices[numExtra - 1], 1),
					mExtraV.subspan(chunkSize() * (numExtra - 1), vPadded()));

			mVole->sendResponse(chl);
		}

		void DotMaliciousLeakyReceiver::processChunk(
			size_t nChunk, size_t numUsed, span<block> messages, block choices)
		{
			size_t blockIdx = mFieldBitsThenBlockIdx++;
			mVole->generateChosen(
				blockIdx, useAES(mVole->mVole.mNumVoles),
				span<block>(&choices, 1), messages.subspan(0, vPadded()));
		}

		template void DotMaliciousLeakySender::sendImpl(
			span<std::array<block, 2>> messages, PRNG& prng, Channel& chl,
			DotMaliciousLeakySender::Hasher& hasher);
		template void DotMaliciousLeakyReceiver::receiveImpl(
			const BitVector& choices, span<block> messages, PRNG& prng, Channel& chl,
			DotMaliciousLeakyReceiver::Hasher& hasher);
		template void DotMaliciousLeakySender::sendImpl(
			span<std::array<block, 2>> messages, PRNG& prng, Channel& chl,
			TwoOneMaliciousSender::Hasher& hasher);
		template void DotMaliciousLeakyReceiver::receiveImpl(
			const BitVector& choices, span<block> messages, PRNG& prng, Channel& chl,
			TwoOneMaliciousReceiver::Hasher& hasher);

	}
}

#endif