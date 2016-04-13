function [theta, J_history] = gradientDescent(X, y, theta, alpha, num_iters)
% GRADIENTDESCENT Performs gradient descent to learn theta
%   theta = GRADIENTDESENT(X, y, theta, alpha, num_iters) updates theta by
%   taking num_iters gradient steps with learning rate alpha

% Initialize some useful values
m = length(y); % number of training examples
J_history = zeros(num_iters, 1);
for iter = 1:num_iters

    % ====================== YOUR CODE HERE ======================
    % Instructions: Perform a single gradient step on the parameter vector
    %               theta.
    %
    % Hint: While debugging, it can be useful to print out the values
    %       of the cost function (computeCost) and gradient here.
    %

    % Calculate hypothesis
    hypothesis = X*theta;

    %disp(sprintf('theta : %0.2f\n',theta));
    %disp(sprintf('hypothesis : %0.2f\n',hypothesis));
    %disp(sprintf('y : %0.2f\n',y));
    theta = theta - ((alpha/m) * ((hypothesis-y)'*X)');



    % ============================================================

    % Save the cost J in every iteration
    J_history(iter) = computeCost(X, y, theta);
    %disp(sprintf('J_history value : %0.2f\n',J_history(iter)));

    %disp(sprintf('theta : %0.2f\n',theta));

end

end
