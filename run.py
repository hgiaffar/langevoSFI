# Run model from command line

from __future__ import print_function
import argparse
import numpy as np
import time

import mutators
import model
from utils import log
from config import git_strings
git_strings.append("@(#)run.py: $Id$")

mutatorclassnames = list(map(lambda x: x.__name__, mutators.VALID_MUTATOR_CLASSES))

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--num_agents", type=int, help="Number of agents", default=10)
parser.add_argument("--num_memes" , type=int, help="Number of memes", default=10)
parser.add_argument("--num_lexes" , type=int, help="Number of lexes", default=10)
parser.add_argument("--num_steps" , type=int, help="Number of steps", default=1000000)
parser.add_argument("--init_meme_prob", type=str, choices=['UNIFORM','RANDOM'], 
                    help="How to initialize meme probabilities", default='UNIFORM')
parser.add_argument("--mutator_class", type=str, choices=mutatorclassnames, help="Mutation operator", default=mutatorclassnames[0])
parser.add_argument("--mutation_scale" , type=float, help="Mutation scale", default=0.1)
parser.add_argument("--temperature", type=float, help="Temperature for acceptance step", default=0)
parser.add_argument("--report_every", type=int, help="How often to log stats", default=10000)
parser.add_argument("--logfile", type=str, help="Output file to log to")

args = parser.parse_args()

# Generate probability of memes (this will be fixed)
if args.init_meme_prob == 'UNIFORM':
  meme_probs = np.ones(args.num_memes) / args.num_memes
elif args.init_meme_prob == 'RANDOM':
  meme_probs = np.random.random(args.num_memes)
  meme_probs /= meme_probs.sum()

grammars = model.init_grammars(meme_probs, 
                               num_agents = args.num_agents, 
                               num_memes  = args.num_memes, 
                               num_lexes  = args.num_lexes)

logfile = args.logfile
if logfile is None:
    print("# logfile not specified, logging only to stdout")

log("# GitHub versions:\n#\t" + "\n#\t".join(git_strings), args.logfile)
log("# %s" % str(args.__dict__), args.logfile)

model.run_simulation(grammars,
                     meme_probs = meme_probs,
                     num_agents = args.num_agents,
                     num_memes  = args.num_memes,
                     num_lexes  = args.num_lexes,
                     num_steps  = args.num_steps,
                     mutator_class = getattr(mutators, args.mutator_class),
                     mutation_scale = args.mutation_scale,
                     temperature = args.temperature,
                     report_every = args.report_every,
                     logfile=args.logfile)
