#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sqlite3
from tqdm import tqdm

source_data = "doubutsu-values-full.txt"
dbfile = "doubutsu.db"
if os.path.isfile(dbfile):
  os.unlink(dbfile)


def _data_generator():
  with open(source_data, "rt") as f:
    for row in tqdm(f):
      row = row.strip().split(" ")
      row = int(row[0]), round(float(row[1]) * 10000)
      yield row


with sqlite3.connect(dbfile) as conn:
  c = conn.cursor()
  q = """
  CREATE TABLE stateValues (
    stateIndex integer PRIMARY KEY NOT NULL,
    value integer NOT NULL
  )
  """
  c.execute(q)
  
  q = "INSERT INTO stateValues VALUES (?,?)"
  rows = _data_generator()
  c.executemany(q, rows)