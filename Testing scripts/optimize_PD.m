%%
%initial cont
addpath(genpath(pwd));
init_cont = PIDcontroller();
init_cont.P = 0.05; init_cont.D = 0.01;


%%
%initial tuning rule
optTuningRule_att=TuningRule;
optTuningRule_att.rule_type=TuningRuleType.pm_based;
optTuningRule_att.beta=-0.7;
optTuningRule_att.pm=20;
optTuningRule_att.c1=0.3849;
optTuningRule_att.c3=0.3817;
optTuningRule_att.pm_min=20;
optTuningRule_att.pm_max=90;
optTuningRule_att.beta_min=-0.9;
optTuningRule_att.beta_max=0.9;

%%
%process and optimization
quad_att=SOIPTD_process;
quad_att.K=351.9020*0.4827; %quad roll
quad_att.tau=0.0149;
quad_att.list_of_T=[0.1055, 0.4109];


quad_att_filter=SOIPTD_process_filter(NaN,10);
quad_att_filter.K=351.9020*0.4827; %quad roll
quad_att_filter.tau=0.0149;
quad_att_filter.list_of_T=[0.1055, 0.4109];
% quad_att.K=2.9;
% quad_att.tau=0.0237;
% quad_att.list_of_T=[0.0321, 1.6886];


%%
%direct PD
quad_att_filter.findOptController(init_cont);

%%
%with tuning rule
quad_att.findOptTuningRule(optTuningRule_att);