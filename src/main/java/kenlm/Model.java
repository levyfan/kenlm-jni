package kenlm;

import java.util.ArrayList;
import java.util.List;

import static kenlm.KenlmJNI.*;

/**
 * Wrapper around lm::ngram::Model.
 */
public class Model implements AutoCloseable {

    private long model;

    /**
     * Load the language model.
     * @param path path to an arpa file or a kenlm binary file.
     * @param config configuration options (see lm/config.hh for documentation)
     */
    public Model(String path, Config config) {
        this.model = ModelLoadVirtual(path, config.getCConfig());
    }

    /**
     * Return the log10 probability of a string.  By default, the string is
     * treated as a sentence.
     *   return log10 p(sentence </s> | <s>)
     *
     * If you do not want to condition on the beginning of sentence, pass
     *   bos = False
     * Never include <s> as part of the string.  That would be predicting the
     * beginning of sentence.  Language models are only supposed to condition
     * on it as context.
     *
     * Similarly, the end of sentence token </s> can be omitted with
     *   eos = False
     * Since language models explicitly predict </s>, it can be part of the
     * string.
     *
     * Examples:
     *
     * #Good: returns log10 p(this is a sentence . </s> | <s>)
     * model.score("this is a sentence .")
     * #Good: same as the above but more explicit
     * model.score("this is a sentence .", bos = True, eos = True)
     *
     * #Bad: never include <s>
     * model.score("<s> this is a sentence")
     * #Bad: never include <s>, even if bos = False.
     * model.score("<s> this is a sentence", bos = False)
     *
     * #Good: returns log10 p(a fragment)
     * model.score("a fragment", bos = False, eos = False)
     *
     * #Good: returns log10 p(a fragment </s>)
     * model.score("a fragment", bos = False, eos = True)
     *
     * #Ok, but bad practice: returns log10 p(a fragment </s>)
     * #Unlike <s>, the end of sentence token </s> can appear explicitly.
     * model.score("a fragment </s>", bos = False, eos = False)
     */
    public float score(String sentence, boolean bos, boolean eos) {
        return ModelScore(model, sentence, bos, eos);
    }

    /**
     * Compute perplexity of a sentence.
     * @param sentence One full sentence to score.  Do not include <s> or </s>.
     */
    public double perplexity(String sentence) {
        return ModelPerplexity(model, sentence);
    }

    /**
     * full_scores(sentence, bos = True, eos = Ture) -> generate full scores (prob, ngram length, oov)
     * @param sentence is a string (do not use boundary symbols)
     * @param bos should kenlm add a bos state
     * @param eos should kenlm add an eos state
     */
    public List<FullScoreReturn> fullScores(String sentence, boolean bos, boolean eos) {
        byte[] ptr = ModelFullScores(model, sentence, bos, eos);

        List<FullScoreReturn> list = new ArrayList<>(ptr.length / FullScoreReturn.SIZE);
        for (int offset = 0; offset < ptr.length; offset += FullScoreReturn.SIZE) {
            list.add(new FullScoreReturn(ptr, offset));
        }
        return list;
    }

    /**
     * Change the given state to a BOS state.
     */
    public void beginSentenceWrite(State state) {
        ModelBeginSentenceWrite(model, state.getCState());
    }

    /**
     * Change the given state to a NULL state.
     */
    public void nullContextWrite(State state) {
        ModelNullContextWrite(model, state.getCState());
    }

    /**
     * Return p(word|in_state) and update the output state.
     * Wrapper around model.BaseScore(in_state, Index(word), out_state)
     * @param in_state the context (defaults to NullContext)
     * @param word the suffix
     * @param out_state
     * @return p(word|state)
     */
    public float baseScore(State in_state, String word, State out_state) {
        return ModelBaseScore(model, in_state.getCState(), word, out_state.getCState());
    }

    /**
     * Wrapper around model.BaseScore(in_state, Index(word), out_state)
     * @param in_state the context (defaults to NullContext)
     * @param word the suffix
     * @param out_state
     * @return FullScoreReturn(word|state)
     */
    public FullScoreReturn baseFullScore(State in_state, String word, State out_state) {
        return new FullScoreReturn(ModelBaseFullScore(model, in_state.getCState(), word, out_state.getCState()), 0);
    }

    @Override
    public void close() {
        ModelDelete(model);
    }
}
