===============================
Nearest Neighbor Classification
===============================


.. todo::

    this has much too much beginner/background/unrelated information
    and should be cleaned up. for example, too much on proteins, too
    much on csv_import (we have a tutorial for that), and no need to
    explain the 0-1-loss in detail, etc.

Background
----------

The nearest neighbor classifier is a simple, still powerful classification algorithm.
Assume that we are given a data set

.. math ::
   S = \{(x_1,y_1),...,(x_l,y_l)\}

and that we want to predict the label *y* of an unseen data point *x*.
The idea of *K* nearest neighbor classification is to look in *S* for
those *K* patterns that are most similar to *x* and to choose *y*
based on their labels.

For details see [DMLNb]_.


Nearest Neighbor Classification in Shark
----------------------------------------

Sample classification problem: Protein fold prediction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Proteins are important building blocks of our body and it is essential
to understand their biological function not only for purely scientific
reasons but also for drug discovery and developing of treatments
schemes against diseases.

It is known that the function of a protein is closely related to its
3D spatial structure. Biologists developed several experimental
methods to determine the 3D structure of a protein including protein
nuclear magnetic resonance (NMR) or X-ray based techniques. However,
these experimental techniques are generally time consuming, slow, and
very expensive.

Against this background, predicting the spatial structure based on the
gene sequence and other information about a protein is an important
task in bioinformatics. Here we consider prediction of the secondary
structure of proteins, that is, their general shape ignoring specific
atomic positions in three-dimensional space. The goal is to assign a
protein to one out of 27 SCOP fold types [DingDubchak2001]_.

We consider classification using descriptions of amino-acid sequences
based on the data provided by [DamoulasGirolami2008]_.  The
data :download:`C.csv <../../../../../examples/Supervised/data/C.csv>` provide a description of the
amino-acid compositions of 695 proteins together with the
corresponding fold type. Each row corresponds to a protein. The first
20 attributes in each line of :download:`C.csv <../../../../../examples/Supervised/data/C.csv>` are the
features (input attributes), the final integer indicates the class
(i.e., the fold type).


Reading, inspecting and splitting the data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, let us read in the data
from the file :download:`C.csv <../../../../../examples/Supervised/data/C.csv>`.
assuming ::


   #include <shark/Data/Csv.h>

   using namespace shark;
   using namespace std;

Then code for reading the data may look like this: ::

	ClassificationDataset data;
	import_csv(data, "C.csv", LAST_COLUMN, " ", "#");

The function ``import_csv`` is used for loading the data from the
files, as described in the :doc:`../concepts/data/import_data` tutorial.  In this
example, the inputs and the labels are combined in a single file. The
argument ``LAST_COLUMN`` specifies that the label is the last entry in
a line. The argument ``" "`` specifies the character separating fields
in each line and ``"#"`` defines the character that marks comments in
the data file (i.e., lines starting with # are ignored).

Let's inspect the data: ::

	cout << "number of data points: " << data.numberOfElements() << " "
	     << "number of classes: " << numberOfClasses(data) << " "
	     << "input dimension: " << inputDimension(data) << endl;

Now we know that we have 694 data points points. Every protein is
described by 20 features/attributes and is assigned to one out of 20 classes.

Next, we split our data into a training and a test set used for
identifying and evaluating the model, respectively.
The command ::

        ClassificationDataset dataTest = splitAtElement(data, 311);

splits the data after the 311th element into two parts. the left part stays in the dataset,
the right part will now be our test set.


Model and learning algorithm
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Efficient look-up of the stored training patterns is crucial if the
nearest neighbor method is applied to large data sets.  Tree lookup algorithms
are an efficient means for this, however they only work in low dimensional spaces,
or high dimensional spaces with low intrinsic dimensionality. For high dimensional
data, tree lookup is inefficient and actually may be slower than just valuating the distance
between every neighbor. Thus we provide an implementation of nearest neighor classifiers which
allow to choose which algorithm to use.

The required header files for the different algorithms required in this tutorial are::

  #include <shark/Models/NearestNeighborClassifier.h>//the classifier
  #include <shark/Algorithms/NearestNeighbors/TreeNearestNeighbors.h>//nearest neeighbor search using trees
  #include <shark/Models/Trees/KDTree.h>//KD tree for nearest neighbor lookup

Since our dataset has low dimensionality, we can use a tree lookup.
The standard choice for a tree is a KD-tree [Bentley1975]_,
which is initialized with the training input data as follows: ::

       KDTree<RealVector> tree(data.inputs());

Now we generate the KNN-Lookup algorithm for this tree::

       TreeNearestNeighbors<RealVector,unsigned int> algorithm(data,&tree);

Now defining and training a *K* nearest neighbor classifier with the algorithm is just a single line ::

       NearestNeighborClassifier<RealVector> KNN(&algorithm,K);

instantiating a  :doxy:`NearestNeighborClassifier` object.

Evaluating the model
^^^^^^^^^^^^^^^^^^^^

After training the model, we can evaluate it.  As a performance
measure, we consider the standard 0-1 loss.  The corresponding risk is
the probability of error, the empirical risk is the average
classification error.  When measured on set of sample patterns, it
simply computes the fraction of wrong predictions.
We define the :doxy:`ZeroOneLoss` for ``unsigned integer`` labels and
apply the classifier to the training and the test data: ::: ::

	ZeroOneLoss<unsigned int> loss;
	Data<unsigned int> prediction = KNN(dataTrain.inputs());
	cout << K << "-KNN on training set accuracy: " << 1. - loss(dataTrain.labels(), prediction) << endl;
	prediction = KNN(dataTest.inputs());
	cout << K << "-KNN on test set accuracy:     " << 1. - loss(dataTest.labels(), prediction) << endl;

Of course, the accuracy is given by one minus the error.
The training accuracy for *K=1* is trivial, but it is interesting to
see how this simple classifier performs compared to random guessing.


Full example program
--------------------

The full example program is
:download:`KNNTutorial.cpp <../../../../../examples/Supervised/KNNTutorial.cpp>`.

Advanced topics
---------------

Not only the Euclidean metric is supported, but also kernel-based
nearest neighbor classification is provided. In this case the
kernel-induced metric is applied. The following lines should give the
same results as the example above, because the :doxy:`LinearKernel`
induces the Euclidean metric::

	LinearKernel<RealVector>  kernel;
	KHCTree<RealVector> tree(data.inputs(), &kernel);
	TreeNearestNeighbors<RealVector,unsigned int> algorithm(data,&tree);
	NearestNeighborClassifier<RealVector> KNN(&algorithm,K);

For high dimensional data, as said before, a tree lookup may not be that efficient. In this case,
we can just use the simple brute force algorithm instead, which is
implemented by :doxy:`SimpleNearestNeighbors`. We need to define the
metric to to measure the distance. This was previously handled
implicitly  by the tree, now we specify a kernel function that induces 
the metric. A  comparable setup to the example above is::

    LinearKernel<> kernel;
    SimpleNearestNeighbors<RealVector,unsigned int> algorithm(data,&kernel);
    NearestNeighborClassifier<RealVector> KNN(&algorithm,K);

Changing the kernel will lead to the same results as
changing it in the KHC Tree.

Often you do not only want the nearest neighbor algorithm to predict
the most promising class label for given input pattern *x*, but an
estimate of the probability that *x* belongs to a certain class.
This is provided by the :doxy:`SoftNearestNeighborClassifier` object.
Nearest neighbor regression is implemented as well.

References
----------

.. [Bentley1975] J.L. Bentley. Multidimensional binary search trees
   used for associative searching. Communications of the ACM,
   18(9):509-517, 1975.

.. [DamoulasGirolami2008] T. Damoulas and M. Girolami.
   Probabilistic multi-class multi-kernel learning: on protein fold
   recognition and remote homology detection. Bioinformatics,
   24(10):1264–1270, 2008.

.. [DingDubchak2001] C.H.Q. Ding and I. Dubchak.  Multi-class
   protein fold recognition using support vector machines and neural
   networks. Bioinformatics, 17(4):349–358, 2001.

.. [DMLNb] C. Igel.
   Data Mining: Lecture Notes, chapter 2, 2011
