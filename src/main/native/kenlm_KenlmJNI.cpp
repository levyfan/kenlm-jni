#include "kenlm_KenlmJNI.h"

#include <boost/tokenizer.hpp>
#include "lm/config.hh"
#include "lm/model.hh"

inline static int32_t getIntB(const int8_t *bb, int bi) {
    return (((bb[bi    ]       ) << 24) |
            ((bb[bi + 1] & 0xff) << 16) |
            ((bb[bi + 2] & 0xff) <<  8) |
            ((bb[bi + 3] & 0xff)      ));
}

inline static void putIntB(int8_t *bb, int bi, int32_t x) {
    bb[bi    ] = static_cast<int8_t>(x >> 24);
    bb[bi + 1] = static_cast<int8_t>(x >> 16);
    bb[bi + 2] = static_cast<int8_t>(x >>  8);
    bb[bi + 3] = static_cast<int8_t>(x      );
}

inline static int64_t getLongB(const int8_t *bb, int bi) {
    return ((((long)bb[bi    ]       ) << 56) |
            (((long)bb[bi + 1] & 0xff) << 48) |
            (((long)bb[bi + 2] & 0xff) << 40) |
            (((long)bb[bi + 3] & 0xff) << 32) |
            (((long)bb[bi + 4] & 0xff) << 25) |
            (((long)bb[bi + 5] & 0xff) << 16) |
            (((long)bb[bi + 6] & 0xff) <<  8) |
            (((long)bb[bi + 7] & 0xff)      ));
}

inline static void putLongB(int8_t *bb, int bi, int64_t x) {
    bb[bi    ] = static_cast<int8_t>(x >> 56);
    bb[bi + 1] = static_cast<int8_t>(x >> 48);
    bb[bi + 2] = static_cast<int8_t>(x >> 40);
    bb[bi + 3] = static_cast<int8_t>(x >> 32);
    bb[bi + 4] = static_cast<int8_t>(x >> 24);
    bb[bi + 5] = static_cast<int8_t>(x >> 16);
    bb[bi + 6] = static_cast<int8_t>(x >>  8);
    bb[bi + 7] = static_cast<int8_t>(x      );
}

inline static float_t getFloatB(const int8_t *bb, int bi) {
    int32_t i = getIntB(bb, bi);
    return *reinterpret_cast<float_t *>(&i);
}

inline static void putFloatB(int8_t *bb, int bi, float_t x) {
    int32_t i = *reinterpret_cast<int32_t *>(&x);
    putIntB(bb, bi, i);
}

static const int FullScoreReturnSize = 19;

static void FullScoreReturn(const lm::FullScoreReturn &ret, int8_t* se, bool oov) {
    putFloatB(se, 0, ret.prob);
    se[4] = ret.ngram_length;
    se[5] = static_cast<unsigned char>(ret.independent_left);
    putLongB(se, 6, ret.extend_left);
    putFloatB(se, 14, ret.rest);
    se[18] = static_cast<unsigned char>(oov);
}

typedef boost::tokenizer<boost::char_separator<char>, StringPiece::const_iterator> Tokenizer;

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ConfigNew
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_kenlm_KenlmJNI_ConfigNew
  (JNIEnv *, jclass) {
      auto *config = new lm::ngram::Config();
      return reinterpret_cast<jlong>(config);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ConfigDelete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_ConfigDelete
  (JNIEnv *, jclass, jlong ptr) {
      auto *config = reinterpret_cast<lm::ngram::Config *>(ptr);
      delete config;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ConfigGetLoadMethod
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_kenlm_KenlmJNI_ConfigGetLoadMethod
  (JNIEnv *, jclass, jlong ptr) {
      auto *config = reinterpret_cast<lm::ngram::Config *>(ptr);
      return config->load_method;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ConfigSetLoadMethod
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_ConfigSetLoadMethod
  (JNIEnv *, jclass, jlong ptr, jint load_method) {
      auto *config = reinterpret_cast<lm::ngram::Config *>(ptr);
      config->load_method = static_cast<util::LoadMethod>(load_method);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelLoadVirtual
 * Signature: (Ljava/lang/String;J)J
 */
JNIEXPORT jlong JNICALL Java_kenlm_KenlmJNI_ModelLoadVirtual
  (JNIEnv *env, jclass, jstring path, jlong ptr) {
      auto *config = reinterpret_cast<lm::ngram::Config *>(ptr);

      const char *file_name = env->GetStringUTFChars(path, nullptr);
      lm::base::Model *model = lm::ngram::LoadVirtual(file_name, *config);
      env->ReleaseStringUTFChars(path, file_name);

      return reinterpret_cast<jlong>(model);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelDelete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_ModelDelete
  (JNIEnv *, jclass, jlong ptr) {
      auto *model = reinterpret_cast<lm::base::Model *>(ptr);
      delete model;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelBeginSentenceWrite
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_ModelBeginSentenceWrite
  (JNIEnv *, jclass, jlong _model, jlong _state) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      auto *state = reinterpret_cast<lm::ngram::State *>(_state);
      model->BeginSentenceWrite(state);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelNullContextWrite
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_ModelNullContextWrite
  (JNIEnv *, jclass, jlong _model, jlong _state) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      auto *state = reinterpret_cast<lm::ngram::State *>(_state);
      model->NullContextWrite(state);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelBaseScore
 * Signature: (JJLjava/lang/String;J)F
 */
JNIEXPORT jfloat JNICALL Java_kenlm_KenlmJNI_ModelBaseScore
  (JNIEnv *env, jclass, jlong _model, jlong _state, jstring _word, jlong _out_state) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      auto *state = reinterpret_cast<lm::ngram::State *>(_state);
      auto *out_state = reinterpret_cast<lm::ngram::State *>(_out_state);

      const char *word = env->GetStringUTFChars(_word, nullptr);
      float ret = model->BaseScore(state, model->BaseVocabulary().Index(word), out_state);
      env->ReleaseStringUTFChars(_word, word);

      return ret;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelBaseFullScore
 * Signature: (JJLjava/lang/String;J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_kenlm_KenlmJNI_ModelBaseFullScore
  (JNIEnv *env, jclass, jlong _model, jlong _state, jstring _word, jlong _out_state) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      auto *state = reinterpret_cast<lm::ngram::State *>(_state);
      auto *out_state = reinterpret_cast<lm::ngram::State *>(_out_state);

      const char *word = env->GetStringUTFChars(_word, nullptr);
      lm::WordIndex wid = model->BaseVocabulary().Index(word);
      lm::FullScoreReturn ret = model->BaseFullScore(state, wid, out_state);
      env->ReleaseStringUTFChars(_word, word);

      int8_t se[FullScoreReturnSize];
      FullScoreReturn(ret, se, wid == 0);

      jbyteArray array = env->NewByteArray(FullScoreReturnSize);
      env->SetByteArrayRegion(array, 0, FullScoreReturnSize, se);
      return array;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelScore
 * Signature: (JLjava/lang/String;ZZ)F
 */
JNIEXPORT jfloat JNICALL Java_kenlm_KenlmJNI_ModelScore
  (JNIEnv *env, jclass, jlong _model, jstring _sentence, jboolean bos, jboolean eos) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      const char *sentence = env->GetStringUTFChars(_sentence, nullptr);
      Tokenizer tokenizer(StringPiece(sentence), boost::char_separator<char>(" "));

      lm::ngram::State state, out_state;
      if (bos) {
          model->BeginSentenceWrite(&state);
      } else {
          model->NullContextWrite(&state);
      }
      float total = 0;
      for (auto iter = tokenizer.begin(); iter != tokenizer.end(); ++iter) {
          total += model->BaseScore(&state, model->BaseVocabulary().Index(*iter), &out_state);
          state = out_state;
      }
      if (eos) {
          total += model->BaseScore(&state, model->BaseVocabulary().EndSentence(), &out_state);
      }

      env->ReleaseStringUTFChars(_sentence, sentence);
      return total;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelPerplexity
 * Signature: (JLjava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_kenlm_KenlmJNI_ModelPerplexity
  (JNIEnv *env, jclass, jlong _model, jstring _sentence) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      const char *sentence = env->GetStringUTFChars(_sentence, nullptr);
      Tokenizer tokenizer(StringPiece(sentence), boost::char_separator<char>(" "));

      lm::ngram::State state, out_state;
      model->BeginSentenceWrite(&state);
      float total = 0;
      int words = 1;
      for (auto iter = tokenizer.begin(); iter != tokenizer.end(); ++iter, ++words) {
          total += model->BaseScore(&state, model->BaseVocabulary().Index(*iter), &out_state);
          state = out_state;
      }
      total += model->BaseScore(&state, model->BaseVocabulary().EndSentence(), &out_state);

      env->ReleaseStringUTFChars(_sentence, sentence);
      return std::pow(10.0, -total / words);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    ModelFullScores
 * Signature: (JLjava/lang/String;ZZ)[B
 */
JNIEXPORT jbyteArray JNICALL Java_kenlm_KenlmJNI_ModelFullScores
  (JNIEnv *env, jclass, jlong _model, jstring _sentence, jboolean bos, jboolean eos) {
      auto *model = reinterpret_cast<lm::base::Model *>(_model);
      const char *sentence = env->GetStringUTFChars(_sentence, nullptr);
      Tokenizer tokenizer(StringPiece(sentence), boost::char_separator<char>(" "));

      lm::ngram::State state, out_state;
      if (bos) {
          model->BeginSentenceWrite(&state);
      } else {
          model->NullContextWrite(&state);
      }

      int size = 0;
      for (auto iter = tokenizer.begin(); iter != tokenizer.end(); ++iter) {
          ++size;
      }
      if (eos) {
          ++size;
      }
      auto *se = new int8_t[size * FullScoreReturnSize];

      auto *bb = se;
      for (auto iter = tokenizer.begin(); iter != tokenizer.end(); ++iter, bb += FullScoreReturnSize) {
          lm::WordIndex wid = model->BaseVocabulary().Index(*iter);
          FullScoreReturn(model->BaseFullScore(&state, wid, &out_state), bb, wid == 0);
          state = out_state;
      }
      if (eos) {
          FullScoreReturn(model->BaseFullScore(&state, model->BaseVocabulary().EndSentence(), &out_state), bb, false);
      }

      env->ReleaseStringUTFChars(_sentence, sentence);
      jbyteArray array = env->NewByteArray(size * FullScoreReturnSize);
      env->SetByteArrayRegion(array, 0, size * FullScoreReturnSize, se);
      delete[] se;
      return array;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    StateNew
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_kenlm_KenlmJNI_StateNew
  (JNIEnv *, jclass) {
      auto *state = new lm::ngram::State();
      return reinterpret_cast<jlong>(state);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    StateDelete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_kenlm_KenlmJNI_StateDelete
  (JNIEnv *, jclass, jlong ptr) {
      auto *state = reinterpret_cast<lm::ngram::State *>(ptr);
      delete state;
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    StateCompare
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_kenlm_KenlmJNI_StateCompare
  (JNIEnv *, jclass, jlong p1, jlong p2) {
      auto *s1 = reinterpret_cast<lm::ngram::State *>(p1);
      auto *s2 = reinterpret_cast<lm::ngram::State *>(p2);
      return s1->Compare(*s2);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    StateEquals
 * Signature: (JJ)Z
 */
JNIEXPORT jboolean JNICALL Java_kenlm_KenlmJNI_StateEquals
  (JNIEnv *, jclass, jlong p1, jlong p2) {
      auto *s1 = reinterpret_cast<lm::ngram::State *>(p1);
      auto *s2 = reinterpret_cast<lm::ngram::State *>(p2);
      return static_cast<jboolean>(s1 == s2);
  }

/*
 * Class:     kenlm_KenlmJNI
 * Method:    StateHashValue
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_kenlm_KenlmJNI_StateHashValue
  (JNIEnv *, jclass, jlong ptr) {
      auto *state = reinterpret_cast<lm::ngram::State *>(ptr);
      return static_cast<jlong>(lm::ngram::hash_value(*state));
  }
