# This file includes functions to calculate statistics of interest on the population

from __future__ import print_function
import numpy as np
from collections import OrderedDict


# Functions for measuring comprehension (i.e., 1-probability of error) between
# two grammars. Each grammar is an [num_memes X num_lexes] numpy array
def get_comprehension(speaker_grammar, listener_grammar):
    # Takes in two grammar arrays, returns a number indicating comprehension
    listener_meme_given_lexe = listener_grammar/(listener_grammar.sum(axis=0) + 1e-16)

    # Comprehension is given by
    #    Trace( speaker_grammar.dot(listener_meme_given_lexe^T) )
    # In fact, this is equal to
    #    sum( speaker_grammar * listener_meme_given_lexe )
    # where * is element-wise multiplication.
    # We use this latter expression, its faster
    return (speaker_grammar*listener_meme_given_lexe).sum()

def get_comprehension_multiple_speakers(speaker_grammars, listener_grammar):
    # Same as calculate_comprehension, but for a list of speaker grammars
    #  (faster that calling get_comprehension many times, since
    #   listener_meme_given_lexe is only computed once)
    listener_meme_given_lexe = listener_grammar/(listener_grammar.sum(axis=0) + 1e-16)
    comprehension_vals = []
    for sg in speaker_grammars:
        comprehension_vals.append( (sg*listener_meme_given_lexe).sum() )
    return comprehension_vals

def get_comprehension_matrix(grammars):
    # Calculate matrix M of comprehension rates between each pair of agents
    # M[a,b] is comprehension when a is speaker and b is listener
    num_agents = len(grammars)
    comprehension_matrix = np.zeros((num_agents,num_agents))
    for i in range(num_agents):
        comprehension_matrix[:,i] += get_comprehension_multiple_speakers(grammars, grammars[i])
    return comprehension_matrix


# Information theoretic statistics - calculated at the current timepoint and between
# the current and previous timepoints
#  TODO change so these stats can be calculated at arbitrary step delay
def information_moment_ordern(ps, n):
    # calculate nth moment of information for distribution ps.  Entropy is first moment of information
    return np.sum((p*((-np.log2(p))**n)) for p in ps if not np.isclose(p, 0))

def nth_moment_information_population(ps, n):
    # Accept as input a mean (population) grammar. Calculate the nth moment of information of the
    # population probability distribution over lexes for each given meme
    nth_moment_population = np.zeros(ps.shape[0])
    for i in range(ps.shape[0]):
        nth_moment_population[i] += information_moment_ordern(ps[i,:],n)
    return nth_moment_population

def relative_information_pq(ps, qs):
    # calculate divergence between rows in ps and qs (conditional probabilities for lexes given meme)
    Z = np.zeros(ps.shape[0])
    for i in range(ps.shape[0]):
        a = np.asarray(ps[i,:])
        b = np.asarray(qs[i,:])
        Z[i] += np.sum(np.where(a != 0, a * np.log(a / (b+1e-16)), 0))
    return Z

def js_divergence(ps, qs):
    # calculate symmetrized divergence. sqrt(JSD) is a metric
    M = 0.5*(ps + qs)
    return 0.5 * (relative_information_pq(ps, M) + relative_information_pq(qs, M))


# Calculate statistics on the population of grammars
def get_grammars_stats(grammars_tensor, old_grammars_tensor, report_level=3):
    # grammars_tensor is [num_memes, num_lexes, num_agents] np.array for current population
    # old_grammars_tensor is same, but for populating during last reporting interval

    stats = OrderedDict()
    grammars =[grammars_tensor[:,:,i] for i in range(grammars_tensor.shape[2])]
    stats['Comprehension'] = get_comprehension_matrix(grammars).mean()

    if report_level >= 2:
        mean_grammar = grammars_tensor.mean(axis=2)
        stats['GrammarVar'] = np.linalg.norm(grammars_tensor-mean_grammar[:,:,None])

        if old_grammars_tensor is not None:
            mean_old_grammar = old_grammars_tensor.mean(axis=2)
            stats['AgentGrammarDrift'] = \
              np.linalg.norm(grammars_tensor - old_grammars_tensor)
            stats['MeanGrammarDrift'] = \
              np.linalg.norm(mean_grammar - mean_old_grammar)

            if report_level >= 3:
                # row normalised population grammars
                mean_grammar /= mean_grammar.sum(axis=1)[:,np.newaxis]
                mean_old_grammar /= mean_old_grammar.sum(axis=1)[:,np.newaxis]

                # entropy of each meme for current timepoint p(l | M = mi)
                stats['Entropy'] = nth_moment_information_population(mean_grammar,1)
                stats['2nd_moment_information'] = nth_moment_information_population(mean_grammar,2)
                stats['3rd_moment_information'] = nth_moment_information_population(mean_grammar,3)

                stats['KL_divergence'] = relative_information_pq(mean_grammar, mean_old_grammar)
                stats['JS_divergence'] = js_divergence(mean_grammar, mean_old_grammar)

        else:
            stats['AgentGrammarDrift'] = np.nan
            stats['MeanGrammarDrift'] = np.nan

    return stats

def format_stat_val(k, v):
    if isinstance(v, int):
        r = '%d'%v
    elif isinstance(v, float):
        r = '%0.5f'%v
    else:
        r = str(v)
    if k == 'Time':
        r += 's'
    return r
