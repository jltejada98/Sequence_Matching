# Data Agnostic Scalable Sequence Analysis

## Background
Detecting similarities and patterns between arbitrary sequences of data is a problem faced in numerous fields. From Bioinformatics to Finance, being able to efficiently identify similarities in a data streams is of huge significance.

This project aims to compare two or more sequences encoded in text files and determine any matching portions above a provided minimum length in a fully concurrent and scalable manner.  In addition to providing the matching strings and their locations in all sequences, the program also determines a matching metric depicting the overall coverage of the found matches in each of the sequenecs.

## Sequence Detection

The program determines matches in sequences by the following rules:

1)  If the length of matching sequence is less than the provided minimum length it is not considered a match. <br />

    Ex: Number of Sequences = 2, Minimum Length = 5 <br />
    Sequence 1: ...K(SEDE)B.... <br />
    Sequence 2: ...A(SEDE)C.... <br />
    -> Although (SEDE) is in both sequences it is less than the minimum length, thus not considered to be a match. <br />


2) If a match is found in some but not all of the provided sequences, it is not considered a match. <br />

Ex: Number of Sequences = 3, Minimum Length = 4 <br />
    Sequence 1: K(SEDE)B <br />
    Sequence 2: A(SEDE)C <br />
    Sequence 3: APEOQLSQ <br />
    -> Although (SEDE) is in sequences 1 and 2 and is above the minimum length, since it is not in in sequence 3, so it is not considered to be a match.


3)  If a match is of the minimum length, but continues to have common characters, the longer match is considered. <br />

    Ex: Number of Sequences = 2, Minimum Length = 3 <br />
    Sequence 1: ...K"(SED)E"B.... <br />
    Sequence 2: ...A"(SED)E"C.... <br />
    -> Although (SED) is in both sequences and is of the minimum length, the match can be extended by adding "E". Thus the reported match is "SEDE" <br />

4)  If a match is found and also to be inside a larger match, both instances of matches are considered. <br />

    Ex: Number of Sequences = 2, Minimum Length = 3 <br />
    Sequence 1: ..."K(CAE)"B............... <br />
    Sequence 2: ...A(CAE)C...N"K(CAE)"G.... <br />

    -> Both (CAE) and "KCAE" are considered separate and distinct matches. With "KCAE" being reported once and(CAE) being reported twice: In A(CAE)C and                          K(CAE)G). <br />

    Ex:<br />
    Minimum Length = 3 <br />
    Sequence 1: ....B"(C<AE)G>"......... <br />
    Sequence 2: ...T<CAE>D...C"(C<AE)G>"K... <br />

    ->Matches: CAE, CAEG, AEG <br />

## Matching Metric

A matching metric is computed for each of sequences individually and a combined metric for all of them. The matching metric is simply the percentage of the sequence covered by non-overlapping matches. (That is, any matches found inside other matches are not counted.) The metric for each measure is the ratio of number of characters in non-overlapping matches and the total length of the particular string. <br />

Ex: Number of Sequences = 2, Minimum Length = 3 <br />
Sequence 1: (KCAE)B  <br />
Sequence 2: A(CAE)U(KCAE)G <br />

->Although CAE and KCAE are matches in string 1 and 2, since CAE is a sub-match of KCAE, only KCAE is the matched portion in each instance. Sequence 1 is of length 5, and has only KCAE as a non-overlapping match of length 4. Sequence 2 is of length 10, and has CAE and KCAE as the non-overlapping matches of with total length of 7. Combining both sequences yields a total number of overlapping characters to 11, and the total length of both strings is 15. <br />

->We obtain the following metrics: <br />
Sequence 1 Matching Metric = 4/5 = 80% <br />
Sequence 2 Matching Metric = 7/10 = 70% <br />
Combined Metric = 11/15 ~ 73.33 %
