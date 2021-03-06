import test from 'ava';
import {SpellChecker} from "./";

/**
 * Loosely compare two arrays
 *
 * @template T
 * @param {test.ContextualTestContext} t
 * @param {Array<T>} value
 * @param {Array<T>} expected
 */
function about_the_same(t, value, expected) {
  value.forEach((entry, index) => {
    t.deepEqual(entry.sort(), expected[index].sort(), `(Comparing index ${index})`);
  });
}


test("constructor works", (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");

  // t.throws(() => SpellChecker(), /Cannot call (\w*) as a function/i);
  t.throws(() => new SpellChecker(), /wrong number of arguments/i);
  t.throws(() => new SpellChecker(2), /should be a string/i);
});

/*
test("spell check signature", async (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");
  t.plan(2);

  spellchecker.suggestions()
    .catch(err => t.regex(err.message, /first argument should be a string/i));

  spellchecker.suggestions(2)
    .catch(err => t.regex(err.message, /first argument should be a string/i));
});
*/

test("no suggestions for correct word", async (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");
  const suggestions = await spellchecker.suggestions("Lákku");

  t.deepEqual(suggestions, false);
});

test("spelling suggestions", async (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");
  const suggestions = await spellchecker.suggestions("akkusativa");

  t.deepEqual(suggestions, ['akkusatiivva', 'akkusatiiva', 'akkusatiivan', 'akkusatiiva-']);
});

test("no spelling suggestions possible", async (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");
  const suggestions = await spellchecker.suggestions("apfelkuchen");

  t.deepEqual(suggestions, []);
});

test("concurrent spelling suggestions", async (t) => {
  const spellchecker = new SpellChecker("etc/se.zhfst");
  const suggestions = await Promise.all([
    spellchecker.suggestions("akkusativa"),
    spellchecker.suggestions("qwert"),
    spellchecker.suggestions("nutella"),
    spellchecker.suggestions("straßenbahn"),
  ]);

  t.deepEqual(suggestions[0].length > 0, true)
  t.deepEqual(suggestions[1].length > 0, true)
  t.deepEqual(suggestions[2].length > 0, true)
  t.deepEqual(suggestions[3].length === 0, true)
});
