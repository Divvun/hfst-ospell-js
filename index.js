"use strict";

var SpellChecker;

try {
  SpellChecker = require("./build/Debug/hfst-ospell").SpellChecker;
} catch (e) {
  SpellChecker = require("./build/Release/hfst-ospell").SpellChecker;
}

if (!Array.prototype.includes) {
  Array.prototype.includes = function includes(item) {
    return this.indexOf(item) > -1;
  }
}

const co = require("co")
const bluebird = require("bluebird")
const _ = require("lodash")

global.Promise = bluebird.Promise

const callbackSuggestions = SpellChecker.prototype.suggestions;

const punctuation = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~".split("")
function withoutPunctuation(alphabet) {
  return alphabet.filter(a => {
    !punctuation.includes(a)
  })
}

function getIndexOfAny(alphabet, string) {
  for (let i = 0; i < string.length; ++i) {
    if (alphabet.includes(string[i])) {
      return i
    }
  }

  return -1
}

function getLastIndexOfAny(alphabet, string) {
  for (let i = string.length-1; i >= 0; --i) {
    if (alphabet.includes(string[i])) {
      return i
    }
  }

  return -1
}

function trimStart(alphabet, word) {
  const index = getIndexOfAny(alphabet, word)

  if (index == -1) {
    return word
  }

  return word.substring(index)
}

function trimEnd(alphabet, word) {
  const index = getIndexOfAny(alphabet, word)

  if (index == -1) {
    return word
  }

  return word.substring(0, index + 1)
}

function trimBoth(alphabet, word) {
  return trimStart(alphabet, trimEnd(alphabet, word))
}

function toFirstUpper(word) {
  return word[0].toUpperCase() + word.substring(1)
}

function wordVariants(speller, word) {
  const alpha = withoutPunctuation(speller.alphabet())

  let base = [
    word,
    trimStart(alpha, word),
    trimEnd(alpha, word),
    trimBoth(alpha, word)
  ]

  // Lower case them all
  base = base.concat(base.map(b => b.toLowerCase()))

  // First upper them all
  base = base.concat(base.map(b => toFirstUpper(b)))

  // Return only unique
  return _.uniqBy(base)
}

const suggestionGenerator = co.wrap(function* (speller, words) {
  const next = (nextWord) => {
    console.log(nextWord)
    return new Promise((resolve, reject) => {
      callbackSuggestions.call(speller, nextWord, (err, res) => {
        if (err) {
          return reject(err);
        }

        return resolve(res);
      })
    })
  }

  for (const variant of words) {
    const res = yield next(variant)

    // Result found!
    if (res === false) {
      return false
    }

    if (res.length > 0) {
      if (isAllCaps(variant)) {
        res = res.map(x => x.toUpperCase())
      } else if (isFirstCap(variant)) {
        res = res.map(x => _.capitalize(x))
      }

      // Also found.
      if (res.indexOf(variant) > -1) {
        return false
      }

      return res.slice(0, 10)
    }
  }

  return []
})

SpellChecker.prototype.suggestions = function suggestions(word) {
  const words = wordVariants(this, word)

  return suggestionGenerator(this, words)
};

module.exports = {
  SpellChecker: SpellChecker
};
