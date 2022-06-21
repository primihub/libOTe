#pragma once
#include <libOTe/config.h>
#ifdef ENABLE_SOFTSPOKEN_OT

#include "DotSemiHonest.h"
#include "libOTe/Vole/SoftSpokenOT/SubspaceVoleMaliciousLeaky.h"
#include <cryptoTools/Common/Aligned.h>
namespace osuCrypto
{
	namespace SoftSpokenOT
	{

		// Uses SubspaceVoleMalicious as a Delta OT.

		class DotMaliciousLeakySender :
			public DotSemiHonestSenderWithVole<SubspaceVoleMaliciousReceiver<RepetitionCode>>,
			private ChunkedReceiver<
			DotMaliciousLeakySender,
			std::tuple<block>
			>
		{
		public:
			using Base = DotSemiHonestSenderWithVole<SubspaceVoleMaliciousReceiver<RepetitionCode>>;

			AlignedUnVector<block> mExtraW;

			struct Hasher :
				public Chunker<
				Hasher,
				std::tuple<std::array<block, 2>>,
				std::tuple<AlignedUnVector<std::array<block, 2>>>
				>
			{
				using ChunkerBase = Chunker<
					Hasher,
					std::tuple<std::array<block, 2>>,
					std::tuple<AlignedUnVector<std::array<block, 2>>>
				>;
				friend ChunkerBase;

				Hasher() : ChunkerBase(this) {}

				void send(PRNG& prng, Channel& chl) {}
				size_t chunkSize() const { return 128; }
				size_t paddingSize() const { return 0; }
				OC_FORCEINLINE void processChunk(
					size_t nChunk, size_t numUsed,
					span<std::array<block, 2>> messages,
					DotMaliciousLeakySender* parent, block* inputW);
			};

			Hasher mHasher;

			DotMaliciousLeakySender(size_t fieldBits = 2, size_t numThreads_ = 1) :
				Base(fieldBits, numThreads_),
				ChunkerBase(this)
			{}

			DotMaliciousLeakySender splitBase()
			{
				throw RTE_LOC; // TODO: unimplemented.
			}

			std::unique_ptr<OtExtSender> split() override
			{
				return std::make_unique<DotMaliciousLeakySender>(splitBase());
			}

			void setBaseOts(
				span<block> baseRecvOts,
				const BitVector& choices,
				PRNG& prng,
				Channel& chl) override
			{
				Base::setBaseOts(baseRecvOts, choices, prng, chl, true);
			}

			virtual void initTemporaryStorage()
			{
				ChunkerBase::initTemporaryStorage();
				mExtraW.resize(2 * chunkSize() + paddingSize());
				mHasher.initTemporaryStorage();
			}

			void send(span<std::array<block, 2>> messages, PRNG& prng, Channel& chl) override
			{
				sendImpl(messages, prng, chl, mHasher);
			}

			// Low level functions.

			template<typename Hasher1>
			void sendImpl(span<std::array<block, 2>> messages, PRNG& prng, Channel& chl, Hasher1& hasher);

			OC_FORCEINLINE void processChunk(
				size_t nChunk, size_t numUsed, span<block> messages);

		private:
			// These functions don't keep information around to compute the hashes.
			using Base::generateRandom;
			using Base::generateChosen;

		protected:
			using ChunkerBase = ChunkedReceiver<
				DotMaliciousLeakySender,
				std::tuple<block>
			>;
			friend ChunkerBase;
			friend ChunkerBase::Base;

			size_t chunkSize() const { return std::max(roundUpTo(wSize(), 2), (size_t)2 * 128); }
			size_t paddingSize() const { return std::max(chunkSize(), wPadded()) - chunkSize(); }
		};

		class DotMaliciousLeakyReceiver :
			public DotSemiHonestReceiverWithVole<SubspaceVoleMaliciousSender<RepetitionCode>>,
			private ChunkedSender<
			DotMaliciousLeakyReceiver,
			std::tuple<block>
			>
		{
		public:
			using Base = DotSemiHonestReceiverWithVole<SubspaceVoleMaliciousSender<RepetitionCode>>;

			AlignedUnVector<block> mExtraV;

			struct Hasher :
				public Chunker<
				Hasher,
				std::tuple<block>,
				std::tuple<AlignedUnVector<block>>
				>
			{
				using ChunkerBase = Chunker<
					Hasher,
					std::tuple<block>,
					std::tuple<AlignedUnVector<block>>
				>;
				friend ChunkerBase;

				Hasher() : ChunkerBase(this) {}

				void recv(Channel& chl) {}
				size_t chunkSize() const { return 128; }
				size_t paddingSize() const { return 0; }
				OC_FORCEINLINE void processChunk(
					size_t nChunk, size_t numUsed,
					span<block> messages, block choices,
					DotMaliciousLeakyReceiver* parent, block* inputV);
			};

			Hasher mHasher;

			DotMaliciousLeakyReceiver(size_t fieldBits = 2, size_t numThreads_ = 1) :
				Base(fieldBits, numThreads_),
				ChunkerBase(this)
			{}

			DotMaliciousLeakyReceiver splitBase()
			{
				throw RTE_LOC; // TODO: unimplemented.
			}

			std::unique_ptr<OtExtReceiver> split() override
			{
				return std::make_unique<DotMaliciousLeakyReceiver>(splitBase());
			}

			void setBaseOts(span<std::array<block, 2>> baseSendOts, PRNG& prng, Channel& chl) override
			{
				Base::setBaseOts(baseSendOts, prng, chl, true);
			}

			virtual void initTemporaryStorage()
			{
				ChunkerBase::initTemporaryStorage();
				mExtraV.resize(2 * chunkSize() + paddingSize());
				mHasher.initTemporaryStorage();
			}

			void receive(const BitVector& choices, span<block> messages, PRNG& prng, Channel& chl) override
			{
				receiveImpl(choices, messages, prng, chl, mHasher);
			}

			// Low level functions.

			template<typename Hasher1>
			void receiveImpl(
				const BitVector& choices, span<block> messages, PRNG& prng, Channel& chl, Hasher1& hasher);

			OC_FORCEINLINE void processChunk(
				size_t nChunk, size_t numUsed, span<block> messages, block choices);

		private:
			// These functions don't keep information around to compute the hashes.
			using Base::generateRandom;
			using Base::generateChosen;

		protected:
			using ChunkerBase = ChunkedSender<
				DotMaliciousLeakyReceiver,
				std::tuple<block>
			>;
			friend ChunkerBase;
			friend ChunkerBase::Base;

			size_t chunkSize() const { return roundUpTo(vSize(), 2); }
			size_t paddingSize() const { return vPadded() - chunkSize(); }
		};

		void DotMaliciousLeakyReceiver::Hasher::processChunk(
			size_t nChunk, size_t numUsed,
			span<block> messages, block choices,
			DotMaliciousLeakyReceiver* parent, block* inputV)
		{
			inputV += nChunk * parent->chunkSize();
			parent->mVole->hash(span<block>(&choices, 1), span<const block>(inputV, parent->vPadded()));

			transpose128(inputV);
			if (messages.data() != inputV)
				memcpy(messages.data(), inputV, numUsed * sizeof(block));
		}

	}
}

#endif