#include <shark/Algorithms/Pegasos.h>
#include <shark/Algorithms/Trainers/McSvmLLWTrainer.h>
#include <shark/Data/Dataset.h>
#include <shark/Data/DataDistribution.h>
#include <shark/Models/Converter.h>
#include <shark/Models/ConcatenatedModel.h>
#include <shark/Models/LinearModel.h>
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h>
#include <iostream>


using namespace shark;


#define noise 1.0
typedef CompressedRealVector VectorType;
typedef CompressedRealMatrix MatrixType;
typedef CompressedRealMatrixRow RowType;


// data generating distribution for our toy
// multi-category classification problem
/// @cond EXAMPLE_SYMBOLS
class Problem : public LabeledDataDistribution<VectorType, unsigned int>
{
public:
	void draw(VectorType& input, unsigned int& label)const
	{
		label = Rng::discrete(0, 4);
		input.resize(1000002);
		input(1000000) = noise * Rng::gauss() + 3.0 * std::cos((double)label);
		input(1000001) = noise * Rng::gauss() + 3.0 * std::sin((double)label);
	}
};
/// @endcond


int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << "required parameters: ell lambda epsilon" << std::endl;
		return 1;
	}

	// experiment settings
	unsigned int dim = 1000002;
	unsigned int classes = 5;
	unsigned int ell = std::atoi(argv[1]);
	double lambda = std::atof(argv[2]);
	double epsilon = std::atof(argv[3]);
	unsigned int tests = 10000;
	std::printf("ell=%d\n", ell);
	std::printf("lambda=%g\n", lambda);
	std::printf("epsilon=%g\n", epsilon);

	// generate a very simple dataset with a little noise
	Problem problem;
	LabeledData<VectorType, unsigned int> training = problem.generateDataset(ell);
	LabeledData<VectorType, unsigned int> test = problem.generateDataset(tests);

	// define the model
	LinearModel<VectorType, RealVector> linear(dim, classes, false, true);
	ArgMaxConverter conv;
	ConcatenatedModel<VectorType, unsigned int> svm = linear >> conv;

	// train the machine
	std::cout << "machine training ..." << std::endl;
	LinearMcSvmLLWTrainer trainer(1.0 / (lambda * ell), epsilon);
	trainer.train(linear, training);
	std::cout << "done." << std::endl;

	// loss measuring classification errors
	ZeroOneLoss<unsigned int> loss;

	Data<unsigned int> output = svm(training.inputs());
	double train_error = loss.eval(training.labels(), output);
	output = svm(test.inputs());
	double test_error = loss.eval(test.labels(), output);

	std::printf("training error=%9.4g    test error=%9.4g\n",
			train_error,
			test_error);
}
