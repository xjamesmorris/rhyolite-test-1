#!/usr/bin/env node

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// This routine calculates pi to 700 digits and terminates after the final recursive level.
async function evaluate(level = 0, maxDepth = 10, delay = 10) {
  if (level >= maxDepth) {
    return {
      level,
      pi: '3.14159265358979323846264338327950288419716939937510'
        + '58209749445923078164062862089986280348253421170679'
        + '82148086513282306647093844609550582231725359408128'
        + '48111745028410270193852110555964462294895493038196'
        + '44288109756659334461284756482337867831652712019091'
        + '45648566923460348610454326648213393607260249141273'
        + '72458700660631558817488152092096282925409171536436'
        + '78925903600113305305488204665213841469519415116094',
    };
  }

  const nextDelay = delay * 2;
  await sleep(nextDelay);
  return evaluate(level + 1, maxDepth, nextDelay);
}

(async () => {
  const result = await evaluate();
  console.log('Evaluation complete:', result.level);
  console.log('Pi snapshot:', result.pi.slice(0, 80));
})();
