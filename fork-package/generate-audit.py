from pathlib import Path
import subprocess,re,json,hashlib,collections
R=Path(__file__).resolve().parent.parent
D=R/'fork-package';old=Path('/home/oui/maya-v1.1.60-port')
def git(*args):return subprocess.check_output(['git',*args],cwd=R,text=True)
patch=git('diff','--binary','--','CMakeLists.txt','serve','src','tests/ds4_undo_log_test.cpp')
(D/'production-v1.1.60.patch').write_text(patch)
flags=collections.defaultdict(lambda:collections.defaultdict(set));hunks=collections.defaultdict(list);f=None
for line in patch.splitlines():
 if line.startswith('diff --git '):f=line.split(' b/',1)[1]
 elif line.startswith('@@ '):hunks[f].append(line)
 elif f and line.startswith(('+','-')) and not line.startswith(('+++','---')):
  for flag in re.findall(r'\b(?:MAYA|STRATA|DS4)_[A-Z][A-Z0-9_]+\b',line):flags[flag][f].add('added' if line[0]=='+' else 'removed')
paths=list(hunks); sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
identity={p:{'reconstructed_sha256':sha(R/p),'live_sha256':sha(old/p) if (old/p).is_file() else None,'match':(old/p).is_file() and sha(R/p)==sha(old/p)} for p in paths}
(D/'source-file-identities.json').write_text(json.dumps(identity,indent=2)+'\n')
lines=['# Complete changed source hunk index','', 'Generated from the complete patch, including the two previously untracked files. Hunk locations refer to the official tag and reconstructed source; the patch contains every exact line change.','']
for p,hs in hunks.items():
 lines+=['## `'+p+'`','']+['- `'+h.replace('`',"'")+'`' for h in hs]+['']
(D/'HUNK-INDEX.md').write_text('\n'.join(lines))
lines=['# Flag and symbol occurrence index','', 'Mechanical inventory of DS4, STRATA and MAYA names appearing in added or removed patch lines. It includes compile-time symbols and comments as well as runtime variables; use FEATURE-FLAGS.md and the referenced source for semantics. Values are the live engine environment snapshot, filtered to these public configuration names.','']
pid=int(subprocess.check_output(['systemctl','--user','show','maya-v1154-iq2xxs.service','-p','MainPID','--value']))
children=Path(f'/proc/{pid}/task/{pid}/children').read_text().split();ep=next(int(c) for c in children if Path(f'/proc/{c}/cmdline').read_bytes().split(b'\0')[0].endswith(b'/maya-ds4'))
env={a.decode():b.decode() for a,b in (x.split(b'=',1) for x in Path(f'/proc/{ep}/environ').read_bytes().split(b'\0') if b'=' in x)}
for name,files in sorted(flags.items()):
 vals=[]
 for n in [name, name.replace('STRATA_DS4_','MAYA_DS4_',1)] if name.startswith('STRATA_DS4_') else [name]:
  if n in env: vals.append(n+'='+env[n])
 lines+=['## `'+name+'`','', 'Live engine: '+(', '.join('`'+v+'`' for v in vals) if vals else 'not present under the listed spelling(s)')+'.','']
 for p,changes in sorted(files.items()):
  source=(R/p).read_text(errors='replace').splitlines()
  matches=[i for i,s in enumerate(source,1) if name in s]
  refs=', '.join(f'[{p}:{i}](../{p}#L{i})' for i in matches[:12]) or 'removed from this file'
  lines.append('- '+', '.join(sorted(changes))+' in '+refs)
 lines.append('')
(D/'FLAG-OCCURRENCES.md').write_text('\n'.join(lines))
public={k:v for k,v in sorted(env.items()) if k.startswith(('DS4_','MAYA_DS4_','STRATA_DS4_','MAYA_LOOP_TOOL_'))}
(D/'production-settings.json').write_text(json.dumps({'capture':'filtered process environment; not the complete environment','unit':'maya-v1154-iq2xxs.service','engine_sha256':sha(Path('/home/oui/maya-v1.1.60-build/maya-ds4')),'settings':public},indent=2)+'\n')
print('files',len(paths),'hunks',sum(map(len,hunks.values())),'flags/symbols',len(flags),'source matches',sum(x['match'] for x in identity.values()))
